synth
=====

A Powerful C++ Templating Framework with a [command-line tool](#command-line) and [Python module](#python), written by [Alvaro J. Genial](http://alva.ro).

[![Build Status](https://travis-ci.org/ajg/synth.png?branch=master)](https://travis-ci.org/ajg/synth)

Synopsis
--------

Synth is a template framework—a set of components that can be mixed and matched to build the right functionality; furthermore, components are loosely-coupled, designed to be both extensible and replaceable.

Status
------

Beta, approaching a first full release.

Motivation
----------

Synth blurs the line between compile-time and runtime, and it does so by blending three worlds: (a) the static C++ type system; (b) the dynamic values that need to be manipulated and formatted, including those from other languages; and (c) the templates to do so. The name is an allusion to this synthesis process, which combines values to generate new ones (streams, files, strings, numbers, etc.)

Examples
--------

### Command-line

```shell
echo '{"user": "Dolph Lundgren"}' > 'context.json'
echo 'Howdy, {{ user }}!' > 'template.txt'

cat template.txt | ./synth -e django -c context.json
```

### Python

```python
import synth, sys

def simple_tmpl_example():
    t = synth.Template('Howdy, <TMPL_VAR user>!', 'tmpl')
    c = {'user': 'Dolph Lundgren'}

    t.render_to_path("greeting.txt", c) # or, e.g.:
    t.render_to_file(sys.stdout, c) # or, e.g.:
    print t.render_to_string(c)
```

### [C++](./examples/simple_ssi.cpp)

```c++
#include <iostream>
#include <ajg/synth.hpp>

namespace synth = ajg::synth;

typedef synth::default_traits<char>                         traits_type;
typedef synth::engines::ssi::engine<traits_type>            engine_type;
typedef synth::templates::string_template<engine_type>      template_type;

int main() {
    template_type const t("Howdy, <!--#echo var=\"user\" -->!");
    template_type::context_type c;
    c["user"] = "Dolph Lundgren";

    // Render to different destinations:
    t.render_to_stream(std::cout);
    t.render_to_path("greeting.txt", c);
    std::cout << t.render_to_string(c);
    return 0;
}
```

Reference
---------

### Command-line

    USAGE: synth [OPTIONS...]
    Options:
      -h,      --help              print help message
      -v,      --version           print library version
      -c file, --context=file      contextual data             *.{ini,json,xml}
      -e name, --engine=name       template engine             {django,ssi,tmpl}
      -a,      --autoescape[=bool] automatically escape values (default: 'true')
      -d path, --directory=path    template location(s)        (default: '.')
      -r text, --replacement=text  replaces missing values     (default: '')

Installation
------------

### Via System Package Manager

#### Using [Homebrew](http://brew.sh/):

    brew install https://raw.github.com/ajg/synth/master/synth.rb
    # Note that you must append `--with-python` to install the Python module.

(Support for other system package managers like Apt or Yum is [welcome](#future-work).)

### Via Python Package Manager

#### Using [Pip](http://www.pip-installer.org/):

    pip install synth # Prefix with `sudo` if needed.

#### Using [Easy Install](https://wiki.python.org/moin/EasyInstall):

    easy_install synth # Prefix with `sudo` if needed.

### Manually

#### From Binaries:

(Pre-built binary releases for common platforms are [welcome](#future-work).)

#### From Source:

 1. Ensure you have the following:
   - A tool to get the source (`git`, `curl`, a browser, etc.)
   - A system to build it with ([SCons](http://scons.org) or Visual Studio)
   - A C++ compiler (`clang`, `gcc`, `msvc`, etc.) In particular, Synth is known to compile with:
     * `gcc` versions `4.2.1`, `4.6.3` and `4.7.2`
     * `clang` versions `3.0` and `3.3`
     * `msvc` version `12.0` (VS2013)
 2. Get the source (e.g. the latest, as shown here):

        git clone --depth 1 --recursive https://github.com/ajg/synth.git && cd synth

 3. *Optionally*, build the [command-line tool](#command-line):

        scons synth # Add debug=1 to generate debugging symbols & disable optimizations.

    (Note that if you are hell bent on it, you can use a different version of Boost; see [Infrequently Asked Questions](#infrequently-asked-questions).)

 4. *Optionally*, build (and install) the [Python module](#python):

        python setup.py install # Prefix with `sudo` if needed.


<!-- TODO: Figure out how to include submodules in tarballs & zipballs.
 2. Get the source (pick your poison):
      - With Git:

            git clone --depth 1 --recursive https://github.com/ajg/synth.git && cd synth
      - With Curl:

            curl -L https://github.com/ajg/synth/archive/master.tar.gz | tar xz && cd synth-master
      - Otherwise, try using https://github.com/ajg/synth/archive/master.zip
-->

Components
----------

### Engines

 - [`django`](#django-engine): An implementation of [Django Templates](https://docs.djangoproject.com/en/dev/topics/templates/).
 - [`ssi`](#ssi-engine): An implementation of [Server Side Includes](http://httpd.apache.org/docs/current/howto/ssi.html).
 - [`tmpl`](#tmpl-engine): An implementation of [Perl's HTML::Template](http://html-template.sourceforge.net/html_template.html).
 - `null`: A minimal implementation that does nothing (i.e. a no-op.)

### Bindings

 - `command_line`
 - `python`

### Templates

 - `char_template`
 - `path_template`
 - `stream_template`
 - `string_template`

### Adapters

 - `array`
   * `[N]` (Native static array.)
   * `[]` (Native dynamic array.)
   * `boost::array`
 - `bool`
   * `bool`
 - `complex`
   * `std::complex`
 - `container`
   * `deque`
     + `std::deque`
   * `list`
     + `std::list`
   * `map`
     + `std::map`
     + `std::multimap`
   * `set`
     + `std::set`
     + `std::multiset`
   * `stack`
     + `std::stack`
   * `vector`
     + `std::vector`
 - `memory`
   * `std::auto_ptr`
 - `none`
   * `boost::none_t`
 - `numeric`
   * `char`
   * `char signed`
   * `char unsigned`
   * `short`
   * `short unsigned`
   * `int`
   * `int unsigned`
   * `long`
   * `long unsigned`
   * `wchar_t`            (When available, unless disabled by `AJG_SYNTH_CONFIG_NO_WCHAR_T`)
   * `long long`          (When available, unless disabled by `AJG_SYNTH_CONFIG_NO_LONG_LONG`)
   * `long long unsigned` (When available, unless disabled by `AJG_SYNTH_CONFIG_NO_LONG_LONG`)
   * `__int64`            (`msvc`-only.)
   * `__int64 unsigned`   (`msvc`-only.)
   * `float`
   * `double`
   * `long double`
 - `optional`
   * `boost::optional`
 - `ptime`
   * `boost::posix_time::ptime`
 - `ptree`
   * `boost::property_tree::ptree`
 - `pointer`
   * `*` (Native pointer.)
 - `ref`
   * `boost::reference_wrapper`
 - `smart_ptr`
   * `boost::scoped_array`
   * `boost::scoped_ptr`
   * `boost::shared_array`
   * `boost::shared_ptr`
 - `string`
   * `std::basic_string`
   * `char*`
   * `char[N]`
   * `wchar_t*`   (When available, unless disabled by `AJG_SYNTH_CONFIG_NO_WCHAR_T`)
   * `wchar_t[N]` (When available, unless disabled by `AJG_SYNTH_CONFIG_NO_WCHAR_T`)
 - `utility`
   * `std::pair`
 - `variant`
   * `boost::variant`

### Input Formats

 - `ini`
 - `json`
 - `xml`

### Base Components

 - `base_adapter`
 - `bindings::base_binding`
 - `engines::base_engine`
 - `templates::base_template`

### Facades

 - `value_facade`

### [Version](./ajg/synth/version.hpp)

 - `AJG_SYNTH_VERSION`        (e.g. `1.2.3`)
 - `AJG_SYNTH_VERSION_MAJOR`  (e.g. `1`)
 - `AJG_SYNTH_VERSION_MINOR`  (e.g. `2`)
 - `AJG_SYNTH_VERSION_PATCH`  (e.g. `3`)
 - `AJG_SYNTH_VERSION_SEQ`    (e.g. `(1)(2)(3)`)
 - `AJG_SYNTH_VERSION_TUPLE`  (e.g. `(1, 2, 3)`)
 - `AJG_SYNTH_VERSION_ARRAY`  (e.g. `(3, (1, 2, 3))`)
 - `AJG_SYNTH_VERSION_STRING` (e.g. `"1.2.3"`)

### [Configuration](./ajg/synth/config.hpp)

 - `AJG_SYNTH_CONFIG_NO_WCHAR_T`        (default: automatically determined)
 - `AJG_SYNTH_CONFIG_NO_LONG_LONG`      (default: automatically determined)
 - `AJG_SYNTH_CONFIG_NO_DEBUG`          (default: automatically determined)
 - `AJG_SYNTH_CONFIG_NO_WINDOWS_H`      (default: not defined)
 - `AJG_SYNTH_CONFIG_DEFAULT_CHAR_TYPE` (default: `char`)
 - `AJG_SYNTH_CONFIG_MAX_FRAMES`        (default: `1024`)
 - `AJG_SYNTH_CONFIG_HANDLE_ASSERT`     (default: `BOOST_ASSERT`)
 - `AJG_SYNTH_CONFIG_HANDLE_EXCEPTION`  (default: `boost::throw_exception`)

Django Engine
-------------

### Tags

 - `django::builtin_tags::autoescape_tag`
 - `django::builtin_tags::block_tag`
 - `django::builtin_tags::comment_tag`
 - `django::builtin_tags::csrf_token_tag`
 - `django::builtin_tags::cycle_tag`
 - `django::builtin_tags::cycle_as_tag`
 - `django::builtin_tags::cycle_as_silent_tag`
 - `django::builtin_tags::debug_tag`
 - `django::builtin_tags::extends_tag`
 - `django::builtin_tags::filter_tag`
 - `django::builtin_tags::firstof_tag`
 - `django::builtin_tags::for_tag`
 - `django::builtin_tags::for_empty_tag`
 - `django::builtin_tags::if_tag`
 - `django::builtin_tags::ifchanged_tag`
 - `django::builtin_tags::ifequal_tag`
 - `django::builtin_tags::ifnotequal_tag`
 - `django::builtin_tags::include_tag`
 - `django::builtin_tags::include_with_tag`
 - `django::builtin_tags::include_with_only_tag`
 - `django::builtin_tags::load_tag`
 - `django::builtin_tags::load_from_tag`
 - `django::builtin_tags::now_tag`
 - `django::builtin_tags::regroup_tag`
 - `django::builtin_tags::spaceless_tag`
 - `django::builtin_tags::ssi_tag`
 - `django::builtin_tags::templatetag_tag`
 - `django::builtin_tags::url_tag`
 - `django::builtin_tags::url_as_tag`
 - `django::builtin_tags::variable_tag`
 - `django::builtin_tags::verbatim_tag`
 - `django::builtin_tags::widthratio_tag`
 - `django::builtin_tags::with_tag`
 - `django::builtin_tags::library_tag`

### Filters

 - `django::builtin_filters::add_filter`
 - `django::builtin_filters::addslashes_filter`
 - `django::builtin_filters::capfirst_filter`
 - `django::builtin_filters::center_filter`
 - `django::builtin_filters::cut_filter`
 - `django::builtin_filters::date_filter`
 - `django::builtin_filters::default_filter`
 - `django::builtin_filters::default_if_none_filter`
 - `django::builtin_filters::dictsort_filter`
 - `django::builtin_filters::dictsortreversed_filter`
 - `django::builtin_filters::divisibleby_filter`
 - `django::builtin_filters::escape_filter`
 - `django::builtin_filters::escapejs_filter`
 - `django::builtin_filters::filesizeformat_filter`
 - `django::builtin_filters::first_filter`
 - `django::builtin_filters::fix_ampersands_filter`
 - `django::builtin_filters::floatformat_filter`
 - `django::builtin_filters::force_escape_filter`
 - `django::builtin_filters::get_digit_filter`
 - `django::builtin_filters::iriencode_filter`
 - `django::builtin_filters::join_filter`
 - `django::builtin_filters::last_filter`
 - `django::builtin_filters::length_filter`
 - `django::builtin_filters::length_is_filter`
 - `django::builtin_filters::linebreaks_filter`
 - `django::builtin_filters::linebreaksbr_filter`
 - `django::builtin_filters::linenumbers_filter`
 - `django::builtin_filters::ljust_filter`
 - `django::builtin_filters::lower_filter`
 - `django::builtin_filters::make_list_filter`
 - `django::builtin_filters::phone2numeric_filter`
 - `django::builtin_filters::pluralize_filter`
 - `django::builtin_filters::pprint_filter`
 - `django::builtin_filters::random_filter`
 - `django::builtin_filters::removetags_filter`
 - `django::builtin_filters::rjust_filter`
 - `django::builtin_filters::safe_filter`
 - `django::builtin_filters::safeseq_filter`
 - `django::builtin_filters::slice_filter`
 - `django::builtin_filters::slugify_filter`
 - `django::builtin_filters::stringformat_filter`
 - `django::builtin_filters::striptags_filter`
 - `django::builtin_filters::time_filter`
 - `django::builtin_filters::timesince_filter`
 - `django::builtin_filters::timeuntil_filter`
 - `django::builtin_filters::title_filter`
 - `django::builtin_filters::truncatechars_filter`
 - `django::builtin_filters::truncatechars_html_filter`
 - `django::builtin_filters::truncatewords_filter`
 - `django::builtin_filters::truncatewords_html_filter`
 - `django::builtin_filters::unordered_list_filter`
 - `django::builtin_filters::upper_filter`
 - `django::builtin_filters::urlencode_filter`
 - `django::builtin_filters::urlize_filter`
 - `django::builtin_filters::urlizetrunc_filter`
 - `django::builtin_filters::wordcount_filter`
 - `django::builtin_filters::wordwrap_filter`
 - `django::builtin_filters::yesno_filter`

### Options

 - `django::options::autoescape`
 - `django::options::nonbreaking_space`
 - `django::options::default_value` (for `TEMPLATE_STRING_IF_INVALID`)
 - `django::options::formats` (for `TIME_FORMAT`, `DATE_FORMAT`, etc.)
 - `django::options::debug` (for `TEMPLATE_DEBUG`)
 - `django::options::directories` (for `TEMPLATE_DIRS`)
 - `django::options::libraries` (for external tags & filters)
 - `django::options::loaders` (for dynamically loading libraries)

SSI Engine
----------

### Tags

 - `ssi::builtin_tags::config_tag`
 - `ssi::builtin_tags::echo_tag`
 - `ssi::builtin_tags::exec_tag`
 - `ssi::builtin_tags::fsize_tag`
 - `ssi::builtin_tags::flastmod_tag`
 - `ssi::builtin_tags::if_tag`
 - `ssi::builtin_tags::include_tag`
 - `ssi::builtin_tags::printenv_tag`
 - `ssi::builtin_tags::set_tag`

### Options

 - `ssi::options::echo_message`
 - `ssi::options::directories`
 - `ssi::options::size_format`
 - `ssi::options::time_format`
 - `ssi::options::error_message`

TMPL Engine
-----------

### Tags

 - `tmpl::builtin_tags::comment_tag` (Technically, part of [`ctpp`](http://ctpp.havoc.ru/en/))
 - `tmpl::builtin_tags::if_tag`
 - `tmpl::builtin_tags::include_tag`
 - `tmpl::builtin_tags::loop_tag`
 - `tmpl::builtin_tags::unless_tag`
 - `tmpl::builtin_tags::variable_tag`

Future Work
-----------

 - Build:
   * Pre-commit script/hook
   * Add `-fsanitize=undefined` when available (GCC 4.9+ so far)
   * Visual Studio 2013:
     + Add project for command-line tool
     + Pass /W4 cleanly
     + Pass /Wall cleanly
   * Create Visual Studio 2012 solution & projects

 - Distribution:
   * RPM package
   * Apt package
   * Yum package
   * MacPort support
   * Standalone pre-built Darwin binaries
   * Standalone pre-built Windows binaries
   * MSI installer

 - Documentation:
   * Produce Boost-compatible documentation
   * Create `conf.py` (et al.) to enable ReadTheDocs

 - Testing:
   * Rewrite the majority of unit tests as a set of `.in`/`.out` files
   * Add exhaustive date/time formatting tests
   * Add way to specify expected failures; re-enable commented out tests

 - Optimization:
   * Compare benefit/cost of `-O`, `-O2`, `-O3` and `-Ofast`
   * Investigate using thread locals instead of statics for medium-expense objects
   * Investigate `-fvisibility-inlines-hidden`
   * Replace `ostream << string(a, b)` constructs with `std::ostream_iterator` + `std::copy`

 - Bindings:
   * Command-line:
     + Allow specifying formats option
     + Allow specifying debug option
     + Allow named input files
     + Allow named output files
     + [v1+] Allow using arbitrary programs as tags
     + [v1+] Allow using arbitrary programs as filters
   * Python:
     + [v1] Set docstrings where appropriate
     + [v1] Turn optional arguments to synth.Template into kwargs
     + [v1+] Support for Python 3
     + Support is_safe, needs_autoescape, and expects_localtime in custom filters
     + Support for non-simple custom tags via token and parser objects
         1. Custom inclusion tags
         2. Custom assignment tags
         3. Custom tags with arbitrary blocks
   * Other:
     + Create `ruby` binding based on [Rice](http://rice.rubyforge.org/)
     + Create `go` binding based on [cgo](http://golang.org/cmd/cgo/)
     + Create `lua` binding
     + Create `node` binding
     + Create `php` binding
     + Create `c` binding
     + [v1+] Create `haskell` binding

 - Engines:
   * Django:
     + Implement multi-level inheritance
     + Option to pre-load libraries, tags and filters
     + Make markers dynamically configurable
     + Consider using hex character entities (vs. named) to match Django's engine
     + Native `i18n` library
     + Native `l10n` library
     + Native `tz` library
     + Native `static` library
   * SSI:
     + Implement additional tags from [Jigsaw](http://www.w3.org/Jigsaw/Doc/User/SSI.html)
   * TMPL:
     + Consider renaming `html`
   * Other:
     + Create `ctpp` engine
     + [v1+] Create `jinja2` engine
     + [v1+] Create `cheetah` engine
     + [v1+] Create `xslt` engine

 - Adapters:
   * Adapter for `boost::tribool`
   * Adapters for `boost::fusion` sequences
   * Adapters for `boost::local_time` types
   * Adapter for `boost::intrusive_ptr`
   * Support for `boost::enable_shared_from_this`
   * [v2] Adapters for `c++11` types
   * [v2] Adapters for `c++14` types

 - Templates:
   * Create new `file_template` (using `FILE* file`)
     + Add `render_to_file` to base_template
     + Add `render_file` to engines
   * Create `descriptor_template` (using `int descriptor`)
     + Add `render_to_descriptor` to base_template
     + Add `render_descriptor` to engines

 - Refactoring:
   * Rename value_facade to base_value
   * Make documentation comments uniformly `///`
   * Replace catching `invalid_method` with dedicated attribute methods
   * Replace `BOOST_ASSERT` with `AJG_SYNTH_ASSERT`
   * Remove as much dead weight from local Boost copy as possible; and/or
   * Replace local version of Boost with minimal, shallow submodules once boostorg/boost is ready
   * Move `render_tag` and `builtin_tags_` to `base_engine::kernel`
   * Remove all no-op tags (e.g. `cycle_as_silent_tag`)
   * Fold all variant tags into their main version (e.g. `load_from_tag`)
   * [v2] Create `c++11`/`c++14` branch
     + Translate macros to variadic templates
     + Replace `BOOST_FOREACH` with new `for` loop
     + Replace `BOOST_STATIC_CONSTANT` with `static const`
     + Replace `BOOST_STATIC_ASSERT` with `static_assert`
     + Replace `boost::assign` use with aggregate initializers
     + Remove complex redundant `typedef`s in favor of `auto`
     + Replace `<boost/cstdint.hpp>` with `<cstdint>`
     + Replace Boost.Random with `<random>`
     + Replace Boost.Atomic with `<atomic>`
     + Consider switching to unordered_map/unordered_set where possible
   * [v1+] Add `AJG_SYNTH_PRAGMA` macro that invokes `_Pragma` or `__pragma` (MSVC) as needed
     + Add `AJG_SYNTH_PRAGMA(once)` to all header files and see if it speeds up compilations
   * [v1+] Sort `#include`s alphabetically
   * [v1+] Run entire C++ codebase through clang-format
   * Change `class` in template signatures to `typename`
   * [v1] Reformat all `operator _()`'s to `operator_()`
   * [v1+] Remove header guard closing comments
   * [v1+] Remove namespace closing comments
   * [v2+] Factor out values & adapters into separate library for generic language interop

Infrequently Asked Questions
----------------------------

 - Q: Can I use a version of [Boost](http://boost.org) other than the one bundled?
   * A: Yes, you can, though note that unless you're already using Boost in your project, there is usually no good reason to; anyway:
     + On most systems, you can build Synth with the system's Boost by passing `boost=system` to `scons`.
     + On Windows, you'll need to edit the project file(s) in Visual Studio and set the include and library directories to point to the existing Boost installation.
     + The [Python module](#python) does not support this option yet.

 - Q: How can I install a system-wide version of [Boost](http://boost.org)?
   * A: Here are some suggestions:
     + Using Homebrew: `brew install boost` (Append `--with-python` for Boost.Python support.)
     + Using Apt: `sudo apt-get install libboost-all-dev`
     + Using Yum: `sudo yum install boost-devel`
     + On Windows, try http://sourceforge.net/projects/boost/files/boost-binaries/

License
-------

This library is distributed under the Boost [LICENSE](./LICENSE_1_0.txt).
