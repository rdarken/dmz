#ifndef DMZ_QT_PLUGIN_CANVAS_ZOOM_MAP_DOT_H
#define DMZ_QT_PLUGIN_CANVAS_ZOOM_MAP_DOT_H

#include <dmzInputObserverUtil.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <QtGui/QFrame>
#include "ui_dmzQtPluginCanvasZoomPanMap.h"


namespace dmz {

   class QtModuleCanvas;
   class QtModuleMap;


   class QtPluginCanvasZoomPanMap :
         public QFrame,
         public Plugin,
         public InputObserverUtil {

      Q_OBJECT

      public:
         QtPluginCanvasZoomPanMap (const PluginInfo &Info, Config &local);
         ~QtPluginCanvasZoomPanMap ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // Input Observer Interface
         virtual void update_channel_state (const UInt32 Channel, const Boolean State);

         virtual void receive_axis_event (
            const UInt32 Channel,
            const InputEventAxis &Value) {;}

         virtual void receive_button_event (
            const UInt32 Channel,
            const InputEventButton &Value) {;}

         virtual void receive_key_event (
            const UInt32 Channel,
            const InputEventKey &Value);

         virtual void receive_mouse_event (
            const UInt32 Channel,
            const InputEventMouse &Value);

         virtual void receive_switch_event (
            const UInt32 Channel,
            const InputEventSwitch &Value) {;}

      protected slots:
         void on_panLeftButton_clicked ();
         void on_panRightButton_clicked ();
         void on_panUpButton_clicked ();
         void on_panDownButton_clicked ();
         void on_zoomAllButton_clicked ();
         void on_zoomInButton_clicked ();
         void on_zoomOutButton_clicked ();
         void on_zoomSlider_valueChanged (int);

         void slot_zoom_changed (int);

      protected:
         virtual bool eventFilter (QObject *obj, QEvent *event);
         void _init (Config &local);

         Log _log;
         Definitions _rti;
         Ui::zoomPanForm _ui;
         QtModuleCanvas *_canvasModule;
         String _canvasModuleName;
         QtModuleMap *_mapModule;
         String _mapModuleName;
         Handle _source;
         Int32 _active;
         Int32 _mouseButton;
         Boolean _ignoreScaleChange;
         Boolean _handScrolling;
         UInt32 _scrollDelta;
         Float32 _zoomMin;
         Float32 _zoomMax;
         Float32 _zoomStep;
         Float32 _zoomDefault;

      private:
         QtPluginCanvasZoomPanMap ();
         QtPluginCanvasZoomPanMap (const QtPluginCanvasZoomPanMap &);
         QtPluginCanvasZoomPanMap &operator= (const QtPluginCanvasZoomPanMap &);
   };
};


#endif // DMZ_QT_PLUGIN_CANVAS_ZOOM_MAP_DOT_H
