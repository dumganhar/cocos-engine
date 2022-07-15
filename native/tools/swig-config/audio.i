%module(target_namespace="jsb") audio

%insert(header_file) %{
#pragma once
#include "bindings/jswrapper/SeApi.h"
#include "bindings/manual/jsb_conversions.h"
#include "audio/include/AudioEngine.h"
%}

%{
#include "bindings/auto/jsb_audio_auto.h"
%}

%ignore cc::AudioEngine::getPCMHeader;
%ignore cc::AudioEngine::getOriginalPCMBuffer;
%ignore cc::AudioEngine::getPCMBufferByFormat;

%import "audio/include/Export.h"
%include "audio/include/AudioEngine.h"
