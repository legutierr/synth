//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_STACK_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_STACK_HPP_INCLUDED

#include <stack>

#include <ajg/synth/adapters/container_adapter.hpp>

namespace ajg {
namespace synth {

//
// specialization for std::stack
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Behavior, class T>
struct adapter<Behavior, std::stack<T> >  : container_adapter<Behavior, std::stack<T> > {
    adapter(std::stack<T> const& adapted) : container_adapter<Behavior, std::stack<T> >(adapted) {}
};

}} // namespace ajg::synth

#endif // AJG_SYNTH_ADAPTERS_STACK_HPP_INCLUDED

