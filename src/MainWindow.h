/* 
 * File:   MainWindow.h
 * Author: allan
 *
 * Created on April 12, 2016, 11:49 PM
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

/*#include <gtk/gtk.h>
#include <gtk/gtkgl.h>*/
#include <gtk/gtk.h>
#include "osggtkdrawingarea.h"

class MainWindow 
{
public:
    
    MainWindow( std::string title );
    
    virtual ~MainWindow();
    
    void show();
    
    OSGGTKDrawingArea& getCanvas();    
    
private:
    
    //CALLBACKS
    static gboolean onDestroy();
    static gboolean onIdle( gpointer pointer );
    
    static gboolean onOpenButtonClicked( GtkWidget* button, gpointer pointer );
    static gboolean onLightingButtonClicked( GtkWidget* button, gpointer pointer );
    static gboolean onWireframeButtonClicked( GtkWidget* button, gpointer pointer );
    static gboolean onBoundariesButtonClicked( GtkWidget* button, gpointer pointer );
    static gboolean onQuitButtonClicked( GtkWidget* button, gpointer pointer );
    static gboolean onAboutButtonClicked( GtkWidget* button, gpointer pointer );
    static gboolean onFairingModeButtonClicked( GtkWidget* button, gpointer pointer );
    
    std::string _title;
    
    OSGGTKDrawingArea _canvas;
    
    GtkWidget* _dialog;
    GtkWidget* _aboutDialog;
    
    //Menu
    GtkWidget* _openButton;
    GtkWidget* _lightingCheckButton;
    GtkWidget* _wireframeCheckButton;
    GtkWidget* _boundariesCheckButton;
    GtkWidget* _quitButton;
    GtkWidget* _aboutButton;
    
    //Fairing
    GtkWidget* _noFairingButton;
    GtkWidget* _scaleFairingButton;
    GtkWidget* _harmonicFairingButton;
    GtkWidget* _secondOrderFairingButton;
};

#endif /* MAINWINDOW_H */

