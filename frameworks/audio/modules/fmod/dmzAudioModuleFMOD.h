#ifndef DMZ_AUDIO_MODULE_FMOD_DOT_H
#define DMZ_AUDIO_MODULE_FMOD_DOT_H

#include <dmzAudioModule.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeHandle.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeTimeSlice.h>
#include <dmzSystemRefCount.h>
#include <dmzTypesBase.h>
#include <dmzTypesHashTableStringTemplate.h>
#include <dmzTypesHashTableHandleTemplate.h>
#include <dmzTypesMatrix.h>
#include <dmzTypesVector.h>

#include <fmod.hpp>
#include <fmod_errors.h>

namespace dmz {

   class AudioModuleFMOD :
         public Plugin,
         public TimeSlice,
         private AudioModule {

      public:
         //! \cond
         AudioModuleFMOD (const PluginInfo &Info, const Config &Local);
         ~AudioModuleFMOD ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level) {;}

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr) {;}

         // TimeSlice Interface
         virtual void update_time_slice (const Float64 TimeDelta);

         // AudioModule Interface
         virtual Handle create_sound (const String &FileName);
         virtual Boolean destroy_sound (const Handle AudioHandle);

         virtual Handle play_sound (
            const Handle AudioHandle,
            const SoundInit &Init,
            const SoundAttributes &Attributes);

         virtual Boolean update_sound (
            const Handle InstanceHandle,
            const SoundAttributes &Attributes);

         virtual Boolean lookup_sound (
            const Handle InstanceHandle,
            SoundInit &init,
            SoundAttributes &attributes);

         virtual Boolean stop_sound (const Handle InstanceHandle);

         virtual Boolean set_mute_all_state (const Boolean Mute);
         virtual Boolean get_mute_all_state (Boolean &mute);

         virtual Handle create_listener (const String &Name);
         virtual Handle lookup_listener (const String &Name);

         virtual Boolean set_listener (
            const Handle ListenerHandle,
            const Vector &Position,
            const Matrix &Orientation,
            const Vector &Velocity);

         virtual Boolean get_listener (
            const Handle ListenerHandle,
            Vector &position,
            Matrix &orientation,
            Vector &velocity);

         virtual Boolean destroy_listener (const Handle ListenerHandle);

      protected:
         struct SoundStruct : public RefCount {

            const String FileName;
            const RuntimeHandle RTHandle;
            FMOD::Sound *sound;
            HashTableHandleTemplate<SoundStruct> &handleTable;
            HashTableStringTemplate<SoundStruct> &nameTable;

            Handle get_handle () const { return RTHandle.get_runtime_handle (); }

            SoundStruct (
                  const String &AbsFilePath,
                  FMOD::Sound *soundData,
                  HashTableHandleTemplate<SoundStruct> &theHandleTable,
                  HashTableStringTemplate<SoundStruct> &theNameTable,
                  RuntimeContext *context) :
                  FileName (AbsFilePath),
                  RTHandle (FileName + ".SoundDataFMOD", context),
                  sound (soundData),
                  handleTable (theHandleTable),
                  nameTable (theNameTable) {;}

            protected:
               ~SoundStruct () { if (sound) { sound->release (); } }

               virtual void _ref_count_is_zero () {

                  handleTable.remove (RTHandle.get_runtime_handle ());
                  nameTable.remove (FileName);

                  delete this;
               }
         };

         struct InstanceStruct {

            const RuntimeHandle RTHandle;
            AudioModuleFMOD &module;

            Float64 defaultFrequency;
            SoundStruct *data;
            FMOD::Channel *channel;

            InstanceStruct *next;

            Handle get_handle () const { return RTHandle.get_runtime_handle (); }

            void reset () {

               defaultFrequency = 1.0;
               if (data) { data->unref (); data = 0; }
               channel = 0;
               next = 0;
            }

            InstanceStruct (
                  const String &FilePath,
                  AudioModuleFMOD &theModule,
                  SoundStruct *soundData,
                  RuntimeContext *context) :
                  RTHandle (FilePath + ".SoundInstanceFMOD", context),
                  module (theModule),
                  defaultFrequency (1.0),
                  data (soundData),
                  channel (0),
                  next (0) {

               if (data) { data->ref (); }
            }

            ~InstanceStruct () { reset (); }
         };

         struct ListenerStruct {

            const RuntimeHandle RTHandle;
            const String Name;
            Int32 index;

            Vector position;
            Matrix orientation;
            Vector velocity;

            Handle get_handle () const { return RTHandle.get_runtime_handle (); }

            ListenerStruct (
                  const String &TheName,
                  const Int32 TheIndex,
                  RuntimeContext *context) :
                  RTHandle (TheName + ".AudioListenerFMOD", context),
                  Name (TheName),
                  index (TheIndex) {;}

            ~ListenerStruct () {;}
         };

         static FMOD_RESULT F_CALLBACK _channel_callback (
            FMOD_CHANNEL *channel,
            FMOD_CHANNEL_CALLBACKTYPE type,
            int command,
            unsigned int commanddata1,
            unsigned int commanddata2);

         void _init (const Config &Local);

         Boolean _error_check (
            const String &HeaderMessage,
            const FMOD_RESULT Error);

         InstanceStruct *_get_new_instance (SoundStruct *soundData);
         void _remove_instance (Handle InstanceHandle);

         Log _log;

         FMOD::System *_system;

         HashTableHandleTemplate<InstanceStruct> _instanceTable;
         HashTableHandleTemplate<SoundStruct> _soundHandleTable;
         HashTableStringTemplate<SoundStruct> _soundNameTable;
         HashTableStringTemplate<ListenerStruct> _listenerNameTable;
         HashTableHandleTemplate<ListenerStruct> _listenerHandleTable;
         HashTableHandleTemplate<ListenerStruct> _listenerIndexTable;

         InstanceStruct *_instanceRecycle;

         Float32 _dopplerScale;
         Float32 _distanceFactor;
         Float32 _rollOffScale;
         Boolean _rightHandedCoordinates;
         Boolean _forceFullDirectSoundHRTF;
         Int32 _maxChannels;
         //! \endcond

      private:
         AudioModuleFMOD (const AudioModuleFMOD &);
         AudioModuleFMOD &operator= (const AudioModuleFMOD &);
   };
};


//! \cond
inline dmz::Boolean
dmz::AudioModuleFMOD::_error_check (
      const String &HeaderMessage,
      const FMOD_RESULT Error) {

   Boolean result (True);

   if (Error != FMOD_OK) {

      result = False;

      _log.error << "FMOD error - " << HeaderMessage << ": "
         << FMOD_ErrorString(Error) << endl;
   }

   return result;
}
//! \endcond

#endif //  DMZ_AUDIO_MODULE_FMOD_DOT_H
