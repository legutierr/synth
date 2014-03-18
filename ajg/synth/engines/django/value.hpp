//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ENGINES_DJANGO_VALUE_HPP_INCLUDED
#define AJG_SYNTH_ENGINES_DJANGO_VALUE_HPP_INCLUDED

#include <vector>
#include <utility>
#include <algorithm>

#include <boost/bind.hpp>
#include <boost/config.hpp>
#include <boost/assert.hpp>
#include <boost/foreach.hpp>
#include <boost/optional.hpp>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include <ajg/synth/value_facade.hpp>
#include <ajg/synth/adapters/numeric.hpp>
#include <ajg/synth/engines/exceptions.hpp>

namespace ajg {
namespace synth {
namespace django {

template <class Char>
struct value : value_facade<Char, value<Char> > {
  public:

    typedef value                               value_type;
    typedef Char                                char_type;
    typedef value_facade<char_type, value_type> base_type;

    typedef typename base_type::size_type       size_type;
    typedef typename base_type::traits_type     traits_type;
    typedef typename base_type::string_type     string_type;
    typedef typename base_type::string_type     token_type;
    typedef typename base_type::boolean_type    boolean_type;
    typedef typename base_type::number_type     number_type;
    typedef typename base_type::datetime_type   datetime_type;
    typedef typename base_type::const_iterator  const_iterator;

    typedef std::vector<value_type>             sequence_type; // TODO: Use the traits' type.

  public:

    AJG_SYNTH_VALUE_CONSTRUCTORS(value, base_type,
        BOOST_PP_COMMA() safe_(false) {})

  public:

    inline void safe(boolean_type const safe) { safe_ = safe; }
    inline boolean_type safe() const { return safe_; }

    inline value_type& mark_unsafe() {
        return this->safe(false), *this;
    }

    inline value_type mark_unsafe() const {
        value_type copy = *this;
        return copy.mark_unsafe();
    }

    inline value_type& mark_safe() {
        return this->safe(true), *this;
    }

    inline value_type mark_safe() const {
        value_type copy = *this;
        return copy.mark_safe();
    }

    template <class Token>
    inline void token(Token const& token) {
        token_ = token_type(token.first, token.second);
    }

    inline token_type const& token() const {
        BOOST_ASSERT(token_);
        return *token_;
    }

    inline boolean_type is_literal() const {
        return token_;
    }

    value_type escape() const {
        // xxx: Should this method escape binary and control characters?
        return detail::escape_entities(this->to_string());
    }

    /*
    const_iterator find_attribute(value_type const& attribute) const {
        try {
            // First try to find the value itself.
            return this->find(attribute);
        }
        catch (bad_method const& method) {
            if (method.name == "find") {
                try {
                    // If that fails, try using the value as an index.
                    return this->at(attribute.count());
                }
                catch (std::exception const&) {
                    // Do nothing, and pass through to the `throw' below,
                    // so that we surface the original `find' failure.
                }
            }

            throw;
        }
    }
    */

    optional<value_type> get_attribute(value_type const& attribute) const {
        try {
            // First try to locate the value as a key.
            return this->index(attribute);
        }
        catch (bad_method const& method) {
            if (method.name == "index") {
                try {
                    // If that fails, try using the value as an index.
                    const_iterator const it = this->at(attribute.count());
                    return it == this->end() ? none : *it;
                }
                catch (std::exception const&) {
                    // Do nothing, and pass through to the `throw' below,
                    // so that we surface the original `index' failure.
                }
            }

            throw;
        }
    }

    value_type must_get_attribute(value_type const& attribute) const {
        if (optional<value_type> attr = this->get_attribute(attribute)) {
            return *attr;
        }
        else {
            throw_exception(missing_attribute(traits_type::template
                transcode<char>(attribute.to_string())));
        }
    }

    value_type sort_by(value_type const& attribute, boolean_type const reverse) const {
        namespace algo = boost::algorithm;

        std::vector<string_type> names;
        string_type const source    = attribute.to_string(),
                          delimiter = detail::text(".");
        algo::split(names, source, algo::is_any_of(delimiter));

        sequence_type trail;
        BOOST_FOREACH(string_type const& name, names) {
            trail.push_back(value_type(name));
        }

        sequence_type result;
        result.reserve(this->length());
        BOOST_FOREACH(value_type const& value, *this) {
            result.push_back(value);
        }

        if (reverse) {
            std::sort(result.rbegin(), result.rend(),
                boost::bind(deep_less, boost::ref(trail), _1, _2));
        }
        else {
            std::sort(result.begin(), result.end(),
                boost::bind(deep_less, boost::ref(trail), _1, _2));
        }
        return result;
    }

  private:

    static boolean_type deep_less( sequence_type const& trail
                                 , value_type           a
                                 , value_type           b
                                 ) {

        // AJG_DUMP(value_type(trail));
        // AJG_DUMP(a);
        // AJG_DUMP(b);

        BOOST_FOREACH(value_type const& attribute, trail) {
            a = a.must_get_attribute(attribute);
            b = b.must_get_attribute(attribute);
            // AJG_DUMP(a);
            // AJG_DUMP(b);
        }

        return a < b;
    }

  private:

    boolean_type safe_;
    optional<token_type> token_;
};

}}} // namespace ajg::synth::django

namespace ajg {
namespace synth {

//
// specialization for above
////////////////////////////////////////////////////////////////////////////////

/*template <class Traits, class Iterator>
struct adapter<Traits, django::value<Iterator> > {
    adapter(django::value<Iterator> const& value);
};*/

    /*: public forwarding_adapter<Traits, django::value<Iterator>, django::value<Iterator> > {

    adapter(scoped_array<T> const& adapted, size_t const length)
        : adapted_(adapted), length_(length) {}
    scoped_array<T> const& adapted_;
    size_t const length_;

    template <class A> A forward() const { return A(cref(*reinterpret_cast<T(*)[]>(adapted_.get())), length_); }
    // T (&get() const)[] { return *reinterpret_cast<T(*)[]>(adapted_.get()); }
    bool valid() const { return adapted_; }
};*/

}} // namespace ajg::synth

#endif // AJG_SYNTH_ENGINES_DJANGO_VALUE_HPP_INCLUDED
