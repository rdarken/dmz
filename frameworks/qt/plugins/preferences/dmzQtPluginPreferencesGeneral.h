#ifndef DMZ_QT_PLUGIN_PREFERENCES_GENERAL_DOT_H
#define DMZ_QT_PLUGIN_PREFERENCES_GENERAL_DOT_H

#include <dmzQtWidget.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesHashTableStringTemplate.h>
#include <QtGui/QFrame>

class QFormLayout;


namespace dmz {

   class QtPluginPreferencesGeneral :
         public QFrame,
         public QtWidget,
         public Plugin,
         public MessageObserver {

   Q_OBJECT

      public:
         QtPluginPreferencesGeneral (const PluginInfo &Info, Config &local);
         ~QtPluginPreferencesGeneral ();

         // QtWidget Interface
         virtual QWidget *get_qt_widget ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // Message Observer Interface
         virtual void receive_message (
            const Message &Type,
            const UInt32 MessageSendHandle,
            const Handle TargetObserverHandle,
            const Data *InData,
            Data *outData);

      public:
         struct WidgetStruct {

            const Handle ValueAttrHandle;
            QWidget *widget;
            Message message;

            WidgetStruct (const Handle AttrHandle) :
               ValueAttrHandle (AttrHandle), widget (0), message () {;}

            virtual ~WidgetStruct () { widget = 0; }

            virtual void init (Config &local) = 0;
            virtual void send_message () = 0;
            virtual void update (const Data &InData) = 0;
         };

      protected Q_SLOTS:
         void _slot_widget_value_changed ();

      protected:

         void _create_properties (Config &list);
         void _init (Config &local);

         Log _log;
         Definitions _defs;
         QFormLayout *_layout;
         Handle _valueAttrHandle;
         HashTableStringTemplate<WidgetStruct> _widgetTable;

      private:
         QtPluginPreferencesGeneral ();
         QtPluginPreferencesGeneral (const QtPluginPreferencesGeneral &);
         QtPluginPreferencesGeneral &operator= (const QtPluginPreferencesGeneral &);

   };
};

#endif // DMZ_QT_PLUGIN_PREFERENCES_GENERAL_DOT_H
