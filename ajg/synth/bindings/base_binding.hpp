//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_BINDINGS_BASE_BINDING_HPP_INCLUDED
#define AJG_SYNTH_BINDINGS_BASE_BINDING_HPP_INCLUDED

#include <ajg/synth/support.hpp>

#include <memory>
#include <vector>
#include <stdexcept>

#include <boost/shared_ptr.hpp>

#include <ajg/synth/engines.hpp>
#include <ajg/synth/adapters.hpp>
#include <ajg/synth/exceptions.hpp>
#include <ajg/synth/detail/text.hpp>

namespace ajg {
namespace synth {
namespace bindings {

template < class Traits
         , template <class E> class Template
         , template <class T> class Django   = engines::django::engine
         , template <class T> class SSI      = engines::ssi::engine
         , template <class T> class TMPL     = engines::tmpl::engine
         >
struct base_binding {
  public:

    typedef base_binding                                                        base_binding_type;
    typedef Traits                                                              traits_type;
    typedef Django<traits_type>                                                 django_engine_type;
    typedef SSI<traits_type>                                                    ssi_engine_type;
    typedef TMPL<traits_type>                                                   tmpl_engine_type;

    typedef Template<django_engine_type>                                        django_template_type;
    typedef Template<ssi_engine_type>                                           ssi_template_type;
    typedef Template<tmpl_engine_type>                                          tmpl_template_type;

    typedef typename django_template_type::context_type                         django_context_type;
    typedef typename ssi_template_type::context_type                            ssi_context_type;
    typedef typename tmpl_template_type::context_type                           tmpl_context_type;

    typedef typename django_template_type::options_type                         django_options_type;
    typedef typename ssi_template_type::options_type                            ssi_options_type;
    typedef typename tmpl_template_type::options_type                           tmpl_options_type;

    typedef django_options_type                                                 options_type;
    typedef typename django_options_type::arguments_type                        arguments_type;
    typedef typename django_options_type::formats_type                          formats_type;
 // typedef typename django_options_type::markers_type                          markers_type;
    typedef typename django_options_type::library_type                          library_type;
    typedef typename django_options_type::libraries_type                        libraries_type;
    typedef typename django_options_type::loader_type                           loader_type;
    typedef typename django_options_type::loaders_type                          loaders_type;
    typedef typename django_options_type::resolver_type                         resolver_type;
    typedef typename django_options_type::resolvers_type                        resolvers_type;

    typedef typename traits_type::boolean_type                                  boolean_type;
    typedef typename traits_type::char_type                                     char_type;
    typedef typename traits_type::string_type                                   string_type;
    typedef typename traits_type::ostream_type                                  ostream_type;
    typedef typename traits_type::paths_type                                    paths_type;

  private:

    typedef detail::text<string_type>                                           text;

  public:

    template <class Source>
    base_binding( Source         const& source
                , string_type    const& engine_name
                , boolean_type   const  autoescape
                , string_type    const& default_value
                , formats_type   const& formats
             // , markers_type   const& markers
                , boolean_type   const& debug
                , paths_type     const& paths
                , libraries_type const& libraries
                , loaders_type   const& loaders
                , resolvers_type const& resolvers
                )
        : django_template_(engine_name == text::literal("django") ? new django_template_type(source) : 0)
        , ssi_template_   (engine_name == text::literal("ssi")    ? new ssi_template_type   (source) : 0)
        , tmpl_template_  (engine_name == text::literal("tmpl")   ? new tmpl_template_type  (source) : 0)
        , django_options_(autoescape, default_value, formats, debug, paths, libraries, loaders, resolvers)
        , ssi_options_(default_value, paths) // TODO: size_format, time_format, formats, debug, error_message, ...
        , tmpl_options_() {                  // TODO: paths, debug, ...

        if (!django_template_ && !ssi_template_ && !tmpl_template_) {
            AJG_SYNTH_THROW(std::invalid_argument("engine_name"));
        }
    }

  protected:

    // TODO: Support post-construction options (maybe using something like combined_options?)
    //       e.g. Options const& options = {django_options_, ssi_options_, tmpl_options_}

    template <class X, class Context>
    void render_to_stream(ostream_type& ostream, Context const& context) const {
             if (django_template_) return django_template_->render_to_stream(ostream, X::template adapt_context<django_context_type>(context), django_options_);
        else if (ssi_template_)    return ssi_template_->render_to_stream(ostream, X::template adapt_context<ssi_context_type>(context), ssi_options_);
        else if (tmpl_template_)   return tmpl_template_->render_to_stream(ostream, X::template adapt_context<tmpl_context_type>(context), tmpl_options_);
        else AJG_SYNTH_THROW(std::logic_error("missing template"));
    }

    template <class X, class Context>
    string_type render_to_string(Context const& context) const {
             if (django_template_) return django_template_->render_to_string(X::template adapt_context<django_context_type>(context), django_options_);
        else if (ssi_template_)    return ssi_template_->render_to_string(X::template adapt_context<ssi_context_type>(context), ssi_options_);
        else if (tmpl_template_)   return tmpl_template_->render_to_string(X::template adapt_context<tmpl_context_type>(context), tmpl_options_);
        else AJG_SYNTH_THROW(std::logic_error("missing template"));
    }

    template <class X, class Context>
    void render_to_path(string_type const& path, Context const& context) const {
             if (django_template_) return django_template_->render_to_path(path, X::template adapt_context<django_context_type>(context), django_options_);
        else if (ssi_template_)    return ssi_template_->render_to_path(path, X::template adapt_context<ssi_context_type>(context), ssi_options_);
        else if (tmpl_template_)   return tmpl_template_->render_to_path(path, X::template adapt_context<tmpl_context_type>(context), tmpl_options_);
        else AJG_SYNTH_THROW(std::logic_error("missing template"));
    }

  private:

    // TODO[c++11]: Use unique_ptr?
    boost::shared_ptr<django_template_type> django_template_;
    boost::shared_ptr<ssi_template_type>    ssi_template_;
    boost::shared_ptr<tmpl_template_type>   tmpl_template_;

    django_options_type django_options_;
    ssi_options_type    ssi_options_;
    tmpl_options_type   tmpl_options_;
};

}}} // namespace ajg::synth::bindings

#endif // AJG_SYNTH_BINDINGS_BASE_BINDING_HPP_INCLUDED
