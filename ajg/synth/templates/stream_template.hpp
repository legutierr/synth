//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_TEMPLATES_STREAM_TEMPLATE_HPP_INCLUDED
#define AJG_SYNTH_TEMPLATES_STREAM_TEMPLATE_HPP_INCLUDED

#include <ajg/synth/templates/base_template.hpp>
#include <ajg/synth/detail/bidirectional_input_stream.hpp>

namespace ajg {
namespace synth {
namespace templates {

template <class Engine>
struct stream_template : base_template<Engine,
      typename detail::bidirectional_input_stream<
          typename Engine::traits_type::istream_type>::iterator> {

  public:

    typedef stream_template                                                     template_type;
    typedef Engine                                                              engine_type;
    typedef typename engine_type::traits_type                                   traits_type;
    typedef typename traits_type::istream_type                                  istream_type;

  private:

    typedef detail::bidirectional_input_stream<istream_type>                    bidi_istream_type;

  public:

    stream_template(istream_type& istream) : istream_(istream), bidi_istream_(istream_) {
        this->reset(this->bidi_istream_.begin(), this->bidi_istream_.end());
    }

    // NOTE: Provided because base_binding can't cope with a mutable source argument yet.
    // TODO: Introduce source_type or such to all templates and pass it via base_template.
    stream_template(istream_type const& istream) : istream_(const_cast<istream_type&>(istream)), bidi_istream_(istream_) {
        this->reset(this->bidi_istream_.begin(), this->bidi_istream_.end());
    }

  public:

    istream_type const& istream() const { return this->istream_; }

  private:

    istream_type&     istream_;
    bidi_istream_type bidi_istream_;
};

}}} // namespace ajg::synth::templates

#endif // AJG_SYNTH_TEMPLATES_STREAM_TEMPLATE_HPP_INCLUDED
