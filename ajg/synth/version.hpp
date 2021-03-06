//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_VERSION_HPP_INCLUDED
#define AJG_SYNTH_VERSION_HPP_INCLUDED

#define AJG_SYNTH_VERSION_MAJOR 0
#define AJG_SYNTH_VERSION_MINOR 59
#define AJG_SYNTH_VERSION_PATCH 0

#define AJG_SYNTH_VERSION        AJG_SYNTH_VERSION_MAJOR.AJG_SYNTH_VERSION_MINOR.AJG_SYNTH_VERSION_PATCH
#define AJG_SYNTH_VERSION_SEQ    (AJG_SYNTH_VERSION_MAJOR)(AJG_SYNTH_VERSION_MINOR)(AJG_SYNTH_VERSION_PATCH)
#define AJG_SYNTH_VERSION_TUPLE  (AJG_SYNTH_VERSION_MAJOR, AJG_SYNTH_VERSION_MINOR, AJG_SYNTH_VERSION_PATCH)
#define AJG_SYNTH_VERSION_ARRAY  (3, AJG_SYNTH_VERSION_TUPLE)
#define AJG_SYNTH_VERSION_STRING AJG_SYNTH_STRINGIZE(AJG_SYNTH_VERSION)

#define AJG_SYNTH_STRINGIZE_(e) #e
#define AJG_SYNTH_STRINGIZE(e)  AJG_SYNTH_STRINGIZE_(e)

#endif // AJG_SYNTH_VERSION_HPP_INCLUDED
