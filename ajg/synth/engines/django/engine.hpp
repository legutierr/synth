//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ENGINES_DJANGO_ENGINE_HPP_INCLUDED
#define AJG_SYNTH_ENGINES_DJANGO_ENGINE_HPP_INCLUDED

#include <ajg/synth/config.hpp>
#include <ajg/synth/vector.hpp>

#include <map>
#include <string>
#include <vector>
#include <ostream>
#include <numeric>
#include <algorithm>

#include <boost/ref.hpp>
#include <boost/foreach.hpp>
#include <boost/optional.hpp>
#include <boost/tokenizer.hpp>
#include <boost/noncopyable.hpp>

#include <boost/algorithm/string/predicate.hpp>

#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/include/algorithm.hpp>
#include <boost/fusion/sequence/intrinsic/size.hpp>
#include <boost/fusion/sequence/intrinsic/at_c.hpp>

#include <ajg/synth/templates.hpp>
#include <ajg/synth/engines/detail.hpp>
#include <ajg/synth/engines/exceptions.hpp>
#include <ajg/synth/engines/base_definition.hpp>
#include <ajg/synth/engines/django/value.hpp>
#include <ajg/synth/engines/django/loader.hpp>
#include <ajg/synth/engines/django/library.hpp>
#include <ajg/synth/engines/django/options.hpp>

namespace ajg {
namespace synth {
namespace django {

using detail::operator ==;
using detail::find_mapped_value;
namespace x = boost::xpressive;

template < class Library = django::default_library
         , class Loader  = django::default_loader
         >
struct engine : detail::nonconstructible {

typedef engine engine_type;

template <class BidirectionalIterator>
struct definition : base_definition< BidirectionalIterator
                                   , definition<BidirectionalIterator>
                                   > {
  private:

    template <class Sequence>
    struct define_sequence;

  public:

    typedef definition                                                          this_type;
    typedef base_definition<BidirectionalIterator, this_type>                   base_type;
    typedef Library                                                             library_type;
    typedef Loader                                                              loader_type;
    typedef typename base_type::id_type                                         id_type;
    typedef typename base_type::size_type                                       size_type;
    typedef typename base_type::char_type                                       char_type;
    typedef typename base_type::match_type                                      match_type;
    typedef typename base_type::regex_type                                      regex_type;
    typedef typename base_type::frame_type                                      frame_type;
    typedef typename base_type::string_type                                     string_type;
    typedef typename base_type::stream_type                                     stream_type;
    typedef typename base_type::symbols_type                                    symbols_type;
    typedef typename base_type::iterator_type                                   iterator_type;
    typedef typename base_type::definition_type                                 definition_type;
    typedef typename base_type::string_regex_type                               string_regex_type;

    typedef library_type                                                        builtin_tags_type;
    typedef builtin_filters<this_type>                                          builtin_filters_type;
    typedef django::value<char_type>                                            value_type;
    typedef options<value_type>                                                 options_type;
    typedef typename value_type::boolean_type                                   boolean_type;
    typedef typename value_type::datetime_type                                  datetime_type;
    typedef typename value_type::duration_type                                  duration_type;
    typedef typename options_type::context_type                                 context_type;
    typedef typename options_type::names_type                                   names_type;
    typedef typename options_type::sequence_type                                sequence_type;
    typedef typename options_type::arguments_type                               arguments_type;
    typedef detail::indexable_sequence< this_type
                                      , builtin_tags_type
                                      , id_type
                                      , detail::create_definitions_extended
                                      >                                         tag_sequence_type;

  private:

    symbols_type keywords_, reserved_;

    struct not_in {
        symbols_type const& symbols;
        explicit not_in(symbols_type const& symbols) : symbols(symbols) {}

        bool operator ()(typename match_type::value_type const& match) const {
            return this->symbols.find(match.str()) == this->symbols.end();
        }
    };

  public:

    std::map<string_type, string_type> markers;

    inline regex_type word(string_type const& n) {
        namespace x = boost::xpressive;
        return x::as_xpr(n) >> x::_b;
    }

    inline regex_type reserved(string_type const& n) {
        namespace x = boost::xpressive;
        this->reserved_.insert(n);
        return x::as_xpr(n) >> x::_b >> *x::_s;
    }

    inline regex_type keyword(string_type const& k) {
        namespace x = boost::xpressive;
        this->keywords_.insert(k);
        return x::as_xpr(k) >> x::_b >> *x::_s;
    }

    inline regex_type xxx(string_type const& k) {
        namespace x = boost::xpressive;
        this->keywords_.insert(k);
        return x::as_xpr(k) >> x::_b;
    }

    inline regex_type yyy(string_type const& k) {
        namespace x = boost::xpressive;
        return x::as_xpr(k) >> x::_b;
    }

    template <char_type C>
    inline static regex_type token() {
        namespace x = boost::xpressive;
        return x::as_xpr(C) >> *x::_s;
    }

    /*
    template <char_type C, char_type D>
    inline static regex_type token() {
        namespace x = boost::xpressive;
        return x::as_xpr(C) >> x::as_xpr(D) >> *x::_s;
    }
    */

    template <typename Char, size_type M, size_type N>
    inline regex_type marker(Char const (&m)[M], Char const (&n)[N]) {
        this->markers[detail::text(n)] = detail::text(m);
        return x::as_xpr(m);
    }

    /*
    inline static regex_type token(char_type const* const s) {
        namespace x = boost::xpressive;
        return x::as_xpr(s) >> *x::_s;
    }
    */

    /*inline static regex_type token(string_type const& t) {
        namespace x = boost::xpressive;
        return x::as_xpr(t) >> *x::_s;
    }*/

    definition()
        : newline        (detail::text("\n"))
        , ellipsis       (detail::text("..."))
        , brace_open     (marker("{",  "openbrace"))
        , brace_close    (marker("}",  "closebrace"))
        , block_open     (marker("{%", "openblock"))
        , block_close    (marker("%}", "closeblock"))
        , comment_open   (marker("{#", "opencomment"))
        , comment_close  (marker("#}", "closecomment"))
        , variable_open  (marker("{{", "openvariable"))
        , variable_close (marker("}}", "closevariable")) {
        using namespace xpressive;
//
// common grammar
////////////////////////////////////////////////////////////////////////////////////////////////////

        identifier
            = ((alpha | '_') >> *_w >> _b)
            ;
        restricted_identifier
            = identifier[ check(not_in(keywords_)) ]
            ;
        unreserved_identifier
            = restricted_identifier[ check(not_in(reserved_)) ]
            ;
        name
            = (id = restricted_identifier) >> *_s
            ;
        names
            = +name
            ;
        unreserved_name
            = (id = unreserved_identifier) >> *_s
            ;
        package
            = (id = (restricted_identifier >> *('.' >> identifier))) >> *_s
            ;
        packages
            = +package
            ;
        none_literal
            = word("None")
            ;
        true_literal
            = word("True")
            ;
        false_literal
            = word("False")
            ;
        boolean_literal
            = true_literal
            | false_literal
            ;
        number_literal
            = !(set= '-','+') >> +_d // Integral part.
                >> !('.' >> +_d)     // Floating part.
                >> !('e' >> +_d)     // Exponent part.
            ;
        string_literal
            = '"'  >> *~as_xpr('"')  >> '"'
            | '\'' >> *~as_xpr('\'') >> '\''
            ;
        variable_literal
            = restricted_identifier
            ;
        literal
            = none_literal
            | boolean_literal
            | number_literal
            | string_literal
            | variable_literal
            ;
        attribution
            = '.' >> identifier
            ;
        subscription
            = token<'['>() >> x::ref(expression) >> ']' // token<']'>()
            ;
        chain
            // TODO: Consider generalizing literal to expression
            = literal >> /* *_s >>*/ *(attribution | subscription) >> *_s
            ;
        unary_operator
            = xxx("not")
            ;
        binary_operator
            = yyy("==")
            | yyy("!=")
            | yyy("<=")
            | yyy(">=")
            | yyy("<")
            | yyy(">")
            | xxx("and")
            | xxx("or")
            | xxx("in")
            | xxx("not") >> *_s >> xxx("in")
            ;
        binary_expression
            = chain >> *(binary_operator >> *_s >> x::ref(expression))
            ;
        unary_expression
            = unary_operator >> *_s >> x::ref(expression)
            ;
        nested_expression
            = token<'('>() >> x::ref(expression) >> token<')'>()
            ;
        expression
            = unary_expression
            | binary_expression
            | nested_expression
            ;
        arguments
            = *expression
            ;
        variables
            = name >> *(token<','>() >> name)
            ;
        filter
            = name >> !(token<':'>() >> chain)
            ;
        filters
            = *(token<'|'>() >> filter) // FIXME: No whitespace allowed around '|'
            ;
        pipeline
            = filter >> *(token<'|'>() >> filter) // FIXME: No whitespace allowed around '|'
            ;
        skipper
            = block_open
            | block_close
            | comment_open
            | comment_close
            | variable_open
            | variable_close
            ;
        nothing
            = as_xpr('\0') // Xpressive barfs when default-constructed.
            ;
        html_namechar
            = ~(set = ' ', '\t', '\n', '\v', '\f', '\r', '>')
            ;
        html_whitespace
            = (set = ' ', '\t', '\n', '\v', '\f', '\r')
            ;
        html_tag
            = '<' >> !as_xpr('/')
                   // The tag's name:
                   >> (s1 = -+html_namechar)
                   // Attributes, if any:
                   >> !(+html_whitespace >> -*~as_xpr('>'))
               >> !as_xpr('/')
            >> '>'
            ;

        this->initialize_grammar();
        fusion::for_each(tags_.definition, detail::construct<detail::element_initializer<this_type> >(*this));
        detail::index_sequence<this_type, tag_sequence_type, &this_type::tags_, tag_sequence_type::size>(*this);
    }

  public:

    template <char_type Delimiter>
    sequence_type split_argument( value_type   const& argument
                                , context_type const& context
                                , options_type const& options
                                ) const {
        typedef boost::char_separator<char_type>                                separator_type;
        typedef typename value_type::token_type                                 token_type;
        typedef typename token_type::const_iterator                             iterator_type;
        typedef boost::tokenizer<separator_type, iterator_type, token_type>     tokenizer_type;
        typedef definition<iterator_type>                                       definition_type;

        BOOST_ASSERT(argument.is_literal());
        token_type const& source = argument.token();
        static char_type const delimiter[2] = { Delimiter, 0 };
        separator_type const separator(delimiter, 0, keep_empty_tokens);
        tokenizer_type const tokenizer(source.begin(), source.end(), separator);
        static definition_type const tokenizable_definition;
        typename definition_type::match_type match;
        sequence_type sequence;

        BOOST_FOREACH(token_type const& token, tokenizer) {
            if (std::distance(token.begin(), token.end()) == 0) {
                sequence.push_back(value_type());
            }
            else if (xpressive::regex_match(token.begin(), token.end(), match, tokenizable_definition.chain)) {
                try {
                    sequence.push_back(tokenizable_definition.evaluate_chain(match, context, options));
                }
                catch (missing_variable const& e) {
                    string_type const string(token.begin(), token.end());

                    if (this->template transcode<char>(string) != e.name) {
                        throw_exception(e);
                    }

                    // A missing variable means an embedded
                    // argument was meant as a string literal.
                    value_type value = string;
                    value.token(match[0]);
                    sequence.push_back(value);
                }
            }
        }

        return sequence;
    }

    string_type extract_string(match_type const& match) const {
        // Handles "string" or 'string'.
        // TODO: Escape sequences, etc.
        string_type const string = match.str();
        return string.substr(1, string.size() - 2);
    }

    names_type extract_names(match_type const& match) const {
        names_type names;
        BOOST_FOREACH(match_type const& name, detail::select_nested(match, this->name)) {
            names.push_back(name[id].str());
        }
        return names;
    }

    void render( stream_type&        stream
               , frame_type   const& frame
               , context_type const& context
               , options_type const& options) const {
        render_block(stream, frame, context, options);
    }

    void render_file( stream_type&        stream
                    , string_type  const& filepath
                    , context_type const& context
                    , options_type const& options
                    ) const {
        typedef file_template<char_type, engine_type> file_template_type;
        std::string const filepath_ = this->template transcode<char>(filepath);
        file_template_type(filepath_, options.directories).render(stream, context, options);
    }

    void render_text( stream_type&        stream
                    , match_type   const& text
                    , context_type const& context
                    , options_type const& options
                    ) const {
        stream << text.str();
    }

    void render_block( stream_type&        stream
                     , match_type   const& block
                     , context_type const& context
                     , options_type const& options
                     ) const {
        BOOST_FOREACH(match_type const& nested, block.nested_results()) {
            render_match(stream, nested, context, options);
        }
    }

    void render_tag( stream_type&        stream
                   , match_type   const& match
                   , context_type const& context
                   , options_type const& options
                   ) const {
        using namespace detail;
        // If there's only _one_ tag, xpressive will not nest the match, so we use it directly.
        match_type const& tag = tag_sequence_type::size == 1 ? match : detail::unnest(match);
        tag_renderer<this_type> const renderer = { *this, stream, tag, context, options };
        must_find_by_index(*this, tags_.definition, tags_.index, tag.regex_id(), renderer);
    }

    void render_match( stream_type&        stream
                     , match_type   const& match
                     , context_type const& context
                     , options_type const& options
                     ) const {
             if (match == text)  render_text(stream, match, context, options);
        else if (match == block) render_block(stream, match, context, options);
        else if (match == tag)   render_tag(stream, match, context, options);
        else throw_exception(std::logic_error("invalid template state"));
    }

    value_type apply_filters( value_type   const& value
                            , match_type   const& match
                            , context_type const& context
                            , options_type const& options
                            ) const {
        value_type result = value;

        BOOST_FOREACH(match_type const& filter, detail::select_nested(match, this->filter)) {
            BOOST_ASSERT(filter == this->filter);
            string_type const& name  = filter(this->name)[id].str();
            match_type  const& chain = filter(this->chain);

            arguments_type arguments;
            if (chain) {
                arguments.first.push_back(this->evaluate_chain(chain, context, options));
            }
            result = this->apply_filter(result, name, arguments, context, options);
        }

        return result;
    }

    value_type apply_filter( value_type     const& value
                           , string_type    const& name
                           , arguments_type const& arguments
                           , context_type   const& context
                           , options_type   const& options
                           ) const {
        typename options_type::filters_type::const_iterator it = options.loaded_filters.find(name);

        if (it != options.loaded_filters.end()) { // Let library filters override built-in ones.
            return it->second(options, &context, value, arguments);
        }

        // TODO: Pass the full arguments, not just the sequential (.first) ones.
        if (typename builtin_filters_type::filter_type const filter = builtin_filters_type::get(name)) {
            return filter(*this, value, arguments.first, context, options);
        }
        throw_exception(missing_filter(this->template transcode<char>(name)));
    }

    value_type evaluate( match_type     const& match
                       , context_type   const& context
                       , options_type   const& options
              // TODO: , arguments_type const& arguments = arguments_type()
                       ) const {
        return evaluate_expression(match, context, options);
    }

    arguments_type evaluate_arguments( match_type    const& args
                                     , context_type  const& context
                                     , options_type  const& options
                                     ) const {
        arguments_type arguments;
        // TODO: Evaluate the full arguments, not just the sequential (.first) ones.
        BOOST_FOREACH(match_type const& arg, args.nested_results()) {
            arguments.first.push_back(this->evaluate_expression(arg, context, options));
        }
        return arguments;
    }

    value_type evaluate_literal( match_type   const& match
                               , context_type const& context
                               , options_type const& options
                               ) const {
        value_type value;
        BOOST_ASSERT(match == this->literal);
        string_type const  string  = match.str();
        match_type  const& literal = detail::unnest(match);

        if (literal == none_literal) {
            value = value_type();
            value.token(literal[0]);
        }
        else if (literal == boolean_literal) {
            match_type const& boolean = detail::unnest(literal);

            if (boolean == true_literal) {
                value = boolean_type(true);
                value.token(literal[0]);
            }
            else if (boolean == false_literal) {
                value = boolean_type(false);
                value.token(literal[0]);
            }
            else {
                throw_exception(std::logic_error("invalid boolean literal"));
            }
        }
        else if (literal == number_literal) {
            value = boost::lexical_cast<typename value_type::number_type>(string);
            value.token(literal[0]);
        }
        else if (literal == string_literal) {
            value = extract_string(literal);
            // Adjust the token by trimming the quotes.
            value.token(std::make_pair(literal[0].first + 1, literal[0].second - 1));
        }
        else if (literal == variable_literal) {
            if (optional<value_type const&> const variable = detail::find_value(string, context)) {
                value = *variable;
                value.token(literal[0]);
            }
            else {
                throw_exception(missing_variable(this->template transcode<char>(string)));
            }
        }
        else {
            throw_exception(std::logic_error("invalid literal"));
        }

        return value;
    }

    value_type evaluate_expression( match_type   const& match
                                  , context_type const& context
                                  , options_type const& options
                                  ) const {
        match_type const& expr = detail::unnest(match);

        if (expr == unary_expression) {
            return evaluate_unary(expr, context, options);
        }
        else if (expr == binary_expression) {
            return evaluate_binary(expr, context, options);
        }
        else if (expr == nested_expression) {
            match_type const& nested = expr(this->expression);
            return evaluate_expression(nested, context, options);
        }
        else {
            throw_exception(std::logic_error("invalid expression"));
        }
    }

    value_type evaluate_unary( match_type   const& match
                             , context_type const& context
                             , options_type const& options
                             ) const {
        BOOST_ASSERT(match == unary_expression);
        string_type const& op      = match(unary_operator).str();
        match_type  const& operand = match(expression);

        if (op == detail::text("not")) {
            return !evaluate_expression(operand, context, options);
        }
        else {
            throw_exception(std::logic_error("invalid unary operator: " + op));
        }
    }

    value_type evaluate_binary( match_type   const& match
                              , context_type const& context
                              , options_type const& options
                              ) const {
        BOOST_ASSERT(match == binary_expression);
        // First, evaluate the first segment, which is
        // always present, and which is always a chain.
        match_type const& chain = match(this->chain);
        value_type value = evaluate_chain(chain, context, options);
        size_type i = 0;
        string_type op;

        BOOST_FOREACH(match_type const& segment, match.nested_results()) {
            if (!i++) continue; // Skip the first segment (the chain.)
            else if (segment == binary_operator) {
                op = segment.str();
                continue;
            }
            else if (!(segment == expression)) {
                throw_exception(std::logic_error("invalid binary expression"));
            }

            if (op == detail::text("==")) {
                value = value == evaluate_expression(segment, context, options);
            }
            else if (op == detail::text("!=")) {
                value = value != evaluate_expression(segment, context, options);
            }
            else if (op == detail::text("<")) {
                value = value < evaluate_expression(segment, context, options);
            }
            else if (op == detail::text(">")) {
                value = value > evaluate_expression(segment, context, options);
            }
            else if (op == detail::text("<=")) {
                value = value <= evaluate_expression(segment, context, options);
            }
            else if (op == detail::text(">=")) {
                value = value >= evaluate_expression(segment, context, options);
            }
            else if (op == detail::text("and")) {
                value = value ? evaluate_expression(segment, context, options) : value;
            }
            else if (op == detail::text("or")) {
                value = value ? value : evaluate_expression(segment, context, options);
            }
            else if (op == detail::text("in")) {
                value_type const elements = evaluate_expression(segment, context, options);
                value = elements.contains(value);
            }
            else if (algorithm::starts_with(op, detail::text("not"))
                  && algorithm::ends_with(op, detail::text("in"))) {
                value_type const elements = evaluate_expression(segment, context, options);
                value = !elements.contains(value);
            }
            else {
                throw_exception(std::logic_error("invalid binary operator: " + op));
            }
        }

        return value;
    }

    value_type evaluate_chain( match_type   const& match
                             , context_type const& context
                             , options_type const& options
                             ) const {
        BOOST_ASSERT(match == this->chain);
        // First, evaluate the first segment, which is
        // always present, and which is always a literal.
        match_type const& lit = match(this->literal);
        value_type value = evaluate_literal(lit, context, options);

        BOOST_FOREACH(match_type const& segment, detail::drop(match.nested_results(), 1)) {
            match_type const& nested = detail::unnest(segment);
            value_type attribute;

            if (segment == subscription) { // i.e. value [ attribute ]
                attribute = evaluate(nested, context, options);
            }
            else if (segment == attribution) { // i.e. value.attribute
                attribute = nested.str();
            }
            else {
                throw_exception(std::logic_error("invalid chain"));
            }

            value = value.must_get_attribute(attribute);
        }

        return value;
    }

    string_type format_datetime( options_type  const& options
                               , string_type   const& format
                               , datetime_type const& datetime
                               ) const {
        typedef std::map<char_type, string_type>            transliterations_type;
        typedef typename transliterations_type::value_type  transliteration_type;

        static transliterations_type const transliterations = boost::assign::list_of<transliteration_type>
            (char_type('%'), detail::text("%%"))
			(char_type('a'), detail::text(AJG_SYNTH_IF_WINDOWS("", "%P"))) // TODO: Periods; implement on Windows.
            (char_type('A'), detail::text("%p"))
            (char_type('b'), detail::text("%b")) // TODO: Lowercase
            (char_type('B'), detail::text(""))   // "Not implemented" per spec.
            (char_type('c'), detail::text("%Y-%m-%dT%H:%M:%S%z"))
            (char_type('d'), detail::text("%d"))
            (char_type('D'), detail::text("%a"))
            (char_type('e'), detail::text("%z"))    // TODO: Ignored with ptimes
            (char_type('E'), detail::text("%B"))    // TODO: Make locale-aware
            (char_type('f'), detail::text("%l:%M")) // TODO: No leading blank, no zero minutes
            (char_type('F'), detail::text("%B"))
            (char_type('g'), detail::text("%l"))    // TODO: No leading blank
            (char_type('G'), detail::text("%k"))    // TODO: No leading blank
            (char_type('h'), detail::text("%I"))
            (char_type('H'), detail::text("%H"))
            (char_type('i'), detail::text("%M"))
            (char_type('I'), detail::text(""))   // TODO: Implement
            (char_type('j'), detail::text(AJG_SYNTH_IF_WINDOWS("", "%e"))) // TODO: No leading blank; implement on Windows.
            (char_type('l'), detail::text("%A"))
            (char_type('L'), detail::text(""))   // TODO: Implement
            (char_type('m'), detail::text("%m"))
            (char_type('M'), detail::text("%b"))
            (char_type('n'), detail::text("%m")) // TODO: No leading zeros
            (char_type('N'), detail::text("%b")) // TODO: Abbreviations/periods
            (char_type('o'), detail::text("%G"))
            (char_type('O'), detail::text(""))   // TODO: Implement
			(char_type('P'), detail::text(AJG_SYNTH_IF_WINDOWS("", "%r"))) // TODO: Periods, no zero minutes, "midnight"/"noon"; implement on Windows.
            (char_type('r'), detail::text("%a, %d %b %Y %T %z"))
            (char_type('s'), detail::text("%S"))
            (char_type('S'), detail::text(""))   // TODO: Implement
            (char_type('t'), detail::text(""))   // TODO: Implement
            (char_type('T'), detail::text(""))   // TODO: Implement
            (char_type('u'), detail::text("%f")) // TODO: No leading period
            (char_type('U'), detail::text(""))   // TODO: Implement
            (char_type('w'), detail::text("%w"))
			(char_type('W'), detail::text(AJG_SYNTH_IF_WINDOWS("", "%V"))) // TODO: No leading zeros; implement on Windows.
            (char_type('y'), detail::text("%y"))
            (char_type('Y'), detail::text("%Y"))
            (char_type('z'), detail::text("%j")) // TODO: No leading zeros
            (char_type('Z'), detail::text(""))   // TODO: Implement
            ;

        std::basic_ostringstream<char_type> stream;
		typename options_type::formats_type::const_iterator const it = options.formats.find(format);
		string_type const original = it == options.formats.end() ? format : it->second;

        // TODO: This might not be UTF8-safe; consider using a utf8_iterator.
		BOOST_FOREACH(char_type const c, original) {
            stream << find_mapped_value(c, transliterations).get_value_or(string_type(1, c));
        }

        return detail::format_time<string_type>(stream.str(), datetime);
    }

    // TODO: Proper, localizable formatting.
    // TODO: Factor out to detail.
    string_type format_duration( options_type  const& options
                               , duration_type const& duration
                               ) const {
        BOOST_STATIC_CONSTANT(size_type, N = 6);

        static size_type const seconds[N] = { 60 * 60 * 24 * 365
                                            , 60 * 60 * 24 * 30
                                            , 60 * 60 * 24 * 7
                                            , 60 * 60 * 24
                                            , 60 * 60
                                            , 60
                                            };
        static string_type const units[N] = { detail::text("year")
                                            , detail::text("month")
                                            , detail::text("week")
                                            , detail::text("day")
                                            , detail::text("hour")
                                            , detail::text("minute")
                                            };

        if (duration.is_negative()) {
            return pluralize_unit(0, units[N - 1], options);
        }

        string_type result;
        size_type const total = duration.total_seconds();
        size_type count = 0, i = 0;

        for (; i < N; ++i) {
            if ((count = total / seconds[i])) {
                break;
            }
        }

        result += pluralize_unit(count, units[i], options);

        if (i + 1 < N) {
            if ((count = (total - (seconds[i] * count)) / seconds[i + 1])) {
                result += string_type(detail::text(", "))
                       +  pluralize_unit(count, units[i + 1], options);
            }
        }

        return result;
    }

    /*inline static string_type nonbreaking(string_type const& s) {
        return algorithm::replace_all_copy(s, detail::text(" "), options.nonbreaking_space);
    }*/

    inline static string_type pluralize_unit( size_type    const  n
                                            , string_type  const& s
                                            , options_type const& options
                                            ) {
        return boost::lexical_cast<string_type>(n) + options.nonbreaking_space + s +
            (n == 1 ? string_type() : string_type(detail::text("s")));
    }

    optional<string_type> get_view_url( value_type     const& view
                                      , arguments_type const& arguments
                                      , context_type   const& context
                                      , options_type   const& options
                                      ) const {
        string_type name = view.to_string();

        BOOST_FOREACH(typename options_type::resolver_type const& resolver, options.resolvers) {
            if (optional<string_type> const& url = resolver->reverse(name, arguments, context, options)) {
                return url;
            }
        }
        return none;
    }

    void load_library( context_type&      context
                     , options_type&      options
                     , string_type const& library
                     , names_type  const* names   = 0
                     ) const {
        loader_.template load<this_type>(context, options, library, names);
    }

    typename options_type::tag_type const& get_tag( string_type  const& name
                                                  , context_type const& context
                                                  , options_type const& options
                                                  ) const {
        typename options_type::tags_type::const_iterator it = options.loaded_tags.find(name);

        if (it != options.loaded_tags.end()) {
            return it->second;
        }
        throw_exception(missing_tag(this->template transcode<char>(name)));
    }

  public:

    string_type const newline;
    string_type const ellipsis;

    regex_type const brace_open;
    regex_type const brace_close;
    regex_type const block_open;
    regex_type const block_close;
    regex_type const comment_open;
    regex_type const comment_close;
    regex_type const variable_open;
    regex_type const variable_close;

    // TODO: Parallelize the formatting:
    regex_type tag, text, block, skipper, nothing;
    regex_type identifier, restricted_identifier, unreserved_identifier;
    regex_type unreserved_name, name, names;
    regex_type package, packages;
    regex_type arguments;
    regex_type variables;
    regex_type filter, filters, pipeline;
    regex_type chain, subscription, attribution;
    regex_type unary_operator, binary_operator;
    regex_type unary_expression, binary_expression, nested_expression, expression;
    regex_type none_literal;
    regex_type true_literal, false_literal, boolean_literal;
    regex_type string_literal, number_literal, variable_literal, literal;

    string_regex_type html_namechar;
    string_regex_type html_whitespace;
    string_regex_type html_tag;

  private:

    tag_sequence_type    tags_;
    loader_type          loader_;

}; // definition

}; // engine

}}} // namespace ajg::synth::django

#endif // AJG_SYNTH_ENGINES_DJANGO_ENGINE_HPP_INCLUDED
