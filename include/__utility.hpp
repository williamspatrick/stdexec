/*
 * Copyright (c) NVIDIA
 *
 * Licensed under the Apache License Version 2.0 with LLVM Exceptions
 * (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *   https://llvm.org/LICENSE.txt
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once

#include <type_traits>

namespace std {
  // Some utilities for manipulating lists of types at compile time
  template <class...>
    struct __types;

  template <class F, class... Args>
    using __meta_invoke = typename F::template __f<Args...>;

  template <template<class...> class List, template<class...> class Fn>
    struct __transform {
      template <class... Args>
        using __f = List<Fn<Args>...>;
    };

  template <template<class...> class List, class Old, class New>
    struct __replace {
      template <class... Args>
        using __f = List<conditional_t<is_same_v<Old, Args>, New, Old>...>;
    };

  template <template<class...> class First, template<class...> class Second>
    struct __compose {
      template <class...Args>
        using __f = Second<First<Args...>>;
    };

  template <template<class...> class List, class... Front>
    struct __bind_front {
      template <class...Args>
        using __f = List<Front..., Args...>;
    };

  template <bool>
    struct __if_ {
      template <class True, class>
        using __f = True;
    };
  template <>
    struct __if_<false> {
      template <class, class False>
        using __f = False;
    };

  template <template <class> class Pred, class True, class False>
    struct __if {
      template <class T>
        using __f = typename __if_<Pred<T>::value>::template __f<True, False>;
    };

  template <template<class...> class List>
    struct __empty {
      template <class>
        using __f = List<>;
    };

  template <template<class...> class List>
    struct __q {
      template <class... Ts>
        using __f = List<Ts...>;
    };

  template <template<class> class F>
    struct __eval2 {
      template <class T>
        using __f = typename F<T>::template __f<T>;
    };

  // For copying cvref from one type to another:
  template <class T>
  T&& __declval() noexcept;

  template <class Member, class Self>
  Member Self::*__memptr(const Self&);

  template <typename Self, typename Member>
  using __member_t = decltype(
      (__declval<Self>() .* __memptr<Member>(__declval<Self>())));

  // For hiding a template type parameter from ADL
  template <class T>
    struct __id {
      struct __t {
        using type = T;
      };
    };
  template <class T>
    using __id_t = typename __id<T>::__t;

  template <class T>
    using __t = typename T::type;

  // For emplacing non-movable types into optionals:
  template <class Fn>
      requires is_nothrow_move_constructible_v<Fn>
    struct __conv {
      Fn __fn_;
      operator invoke_result_t<Fn> () && {
        return ((Fn&&) __fn_)();
      }
    };
  template <class Fn>
    __conv(Fn) -> __conv<Fn>;
}