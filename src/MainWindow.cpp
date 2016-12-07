/* 
 * File:   MainWindow.cpp
 * Author: allan
 * 
 * Created on April 12, 2016, 11:49 PM
 */

#include "MainWindow.h"
#include "MeshCompletionApplication.h"
#include <string>
#include <iostream>

MainWindow::MainWindow( std::string title ) :
    _title( title ) 
{
    GError* error = NULL;
    GtkBuilder* builder = gtk_builder_new();
     
    std::string gladePath( g_get_current_dir() );
    gladePath.append( "/data/glade/MainWindow.glade" );
    
    if( !gtk_builder_add_from_file( builder, gladePath.c_str(), &error ) )
    {
        g_warning( "Nao foi possivel abrir o arquivo: %s", error->message );
        g_error_free( error );
    }
    
    // Widgets
    _dialog = GTK_WIDGET( gtk_builder_get_object( builder, "window1" ) );
    
    gtk_window_set_title( GTK_WINDOW( _dialog ), _title.c_str() );
    
    GtkWidget* canvasBox = GTK_WIDGET( gtk_builder_get_object( builder, "alignmentCanvas" ) );
    
    if( _canvas.createWidget( 800, 800 ) ) 
    {
        gtk_box_pack_start( GTK_BOX( canvasBox ), _canvas.getWidget(), true, true, 2 );        
    }
    
    _aboutDialog = GTK_WIDGET( gtk_builder_get_object( builder, "aboutdialog1" ) );      
    _openButton = GTK_WIDGET( gtk_builder_get_object( builder, "imagemenuitem2" ) );
    _quitButton = GTK_WIDGET( gtk_builder_get_object( builder, "imagemenuitem5" ) );
    _aboutButton = GTK_WIDGET( gtk_builder_get_object( builder, "imagemenuitem10" ) );
    _lightingCheckButton = GTK_WIDGET( gtk_builder_get_object( builder, "checkmenuitem2" ) );
    _wireframeCheckButton = GTK_WIDGET( gtk_builder_get_object( builder, "checkmenuitem1" ) );
    
    _noFairingButton = GTK_WIDGET( gtk_builder_get_object( builder, "radiomenuitem1" ) );
    _scaleFairingButton = GTK_WIDGET( gtk_builder_get_object( builder, "radiomenuitem2" ) );
    _harmonicFairingButton = GTK_WIDGET( gtk_builder_get_object( builder, "radiomenuitem3" ) );
    _secondOrderFairingButton = GTK_WIDGET( gtk_builder_get_object( builder, "radiomenuitem4" ) );
    
    
    // Signals
    g_timeout_add( 15, (GSourceFunc)( &MainWindow::onIdle ), this );
    
    g_signal_connect( G_OBJECT( _dialog ), "destroy", G_CALLBACK( &MainWindow::onDestroy ), NULL );
    g_signal_connect( G_OBJECT( _dialog ), "delete_event", G_CALLBACK( &MainWindow::onDestroy ), NULL );
        
    g_signal_connect( G_OBJECT( _openButton ), "activate", G_CALLBACK( &MainWindow::onOpenButtonClicked ), _dialog );
    g_signal_connect( G_OBJECT( _quitButton ), "activate", G_CALLBACK( &MainWindow::onQuitButtonClicked ), _dialog );
    g_signal_connect( G_OBJECT( _aboutButton ), "activate", G_CALLBACK( &MainWindow::onAboutButtonClicked ), _dialog );
    g_signal_connect( G_OBJECT( _lightingCheckButton ), "toggled", G_CALLBACK( &MainWindow::onLightingButtonClicked ), _dialog );
    g_signal_connect( G_OBJECT( _wireframeCheckButton ), "toggled", G_CALLBACK( &MainWindow::onWireframeButtonClicked ), _dialog );
    
    g_signal_connect( G_OBJECT( _noFairingButton ), "activate", G_CALLBACK( &MainWindow::onFairingModeButtonClicked ), _dialog );
    g_signal_connect( G_OBJECT( _scaleFairingButton ), "activate", G_CALLBACK( &MainWindow::onFairingModeButtonClicked ), _dialog );
    g_signal_connect( G_OBJECT( _harmonicFairingButton ), "activate", G_CALLBACK( &MainWindow::onFairingModeButtonClicked ), _dialog );
    g_signal_connect( G_OBJECT( _secondOrderFairingButton ), "activate", G_CALLBACK( &MainWindow::onFairingModeButtonClicked ), _dialog );
    
    g_object_set_data( ( GObject* ) _dialog, "THIS", ( gpointer )this );
}


MainWindow::~MainWindow() 
{
    g_object_unref( G_OBJECT( _dialog ) );
}

void MainWindow::show()
{
    gtk_widget_show_all( GTK_WIDGET( _dialog ) );
}

OSGGTKDrawingArea& MainWindow::getCanvas()
{
    return _canvas;
}

gboolean MainWindow::onDestroy()
{
    gtk_main_quit();
    
    return FALSE;
}

gboolean MainWindow::onIdle( gpointer pointer )
{
    MainWindow* dialog = reinterpret_cast< MainWindow* >( pointer );
    
    dialog->_canvas.queueDraw();
    
    return TRUE;
}

gboolean MainWindow::onOpenButtonClicked( GtkWidget* button, gpointer pointer )
{
    gpointer result = g_object_get_data( ( GObject* ) pointer, "THIS" );
    
    if( result == NULL )
        return FALSE;
    
    //MainWindow* dialog = reinterpret_cast< MainWindow* >( result );
    
    MeshCompletionApplication::getInstance()->openFile( "/home/v/allanws/Pessoais/Mestrado/GMP/Trab1/data/bunny_hole.off" );
            
    /*GtkWidget* chooseDialog;
    chooseDialog = gtk_file_chooser_dialog_new( "Open File",
                                          GTK_WINDOW( dialog->_dialog ),
                                          GTK_FILE_CHOOSER_ACTION_OPEN,
                                          GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                          GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
                                          NULL );
    
    if( gtk_dialog_run( GTK_DIALOG( chooseDialog ) ) == GTK_RESPONSE_ACCEPT )
    {
        char *filename;
        filename = gtk_file_chooser_get_filename( GTK_FILE_CHOOSER( chooseDialog ) );        
        std::string filenameStr( filename );
        gtk_window_set_title( GTK_WINDOW( dialog->_dialog ), dialog->_title.c_str() );
        
        if( MeshCompletionApplication::getInstance()->openFile( filenameStr ) )
        {
            std::string newTitle( dialog->_title + " - " + filenameStr );
            gtk_window_set_title( GTK_WINDOW( dialog->_dialog ), newTitle.c_str() );
        }
        
        g_free( filename );
    }
    
    gtk_widget_destroy( chooseDialog );*/
    
    return TRUE;
}

gboolean MainWindow::onQuitButtonClicked( GtkWidget* button, gpointer pointer )
{
    return MainWindow::onDestroy();
}

gboolean MainWindow::onAboutButtonClicked( GtkWidget* button, gpointer pointer )
{
    gpointer result = g_object_get_data( ( GObject* ) pointer, "THIS" );
    
    if( result == NULL )
        return FALSE;
    
    MainWindow* dialog = reinterpret_cast< MainWindow* >( result );
    
    std::string aboutString = 
        "Trabalho sobre correção de malha.\n"
        "Prof. Marcelo Gattass\n"
        "\n"
        "LEFT MOUSE = Rotação\n"
        "RIGHT MOUSE = Zoom\n"
        "MIDDLE MOUSE = Pick/Pan\n"
        "SPACE = Camera inicial\n";
            
    gtk_widget_destroy( dialog->_aboutDialog );
    
    dialog->_aboutDialog = gtk_about_dialog_new();
    gtk_about_dialog_set_name( GTK_ABOUT_DIALOG( dialog->_aboutDialog ), "[GMP] Trabalho 1" );
    gtk_about_dialog_set_version( GTK_ABOUT_DIALOG( dialog->_aboutDialog ), "v1.0" );
    gtk_about_dialog_set_comments( GTK_ABOUT_DIALOG( dialog->_aboutDialog ), aboutString.c_str() );
    //gtk_about_dialog_set_authors( GTK_ABOUT_DIALOG( dialog->_aboutDialog ), authorName );
    
    gtk_widget_show_all( dialog->_aboutDialog );
    
    return TRUE;
}

gboolean MainWindow::onLightingButtonClicked( GtkWidget* button, gpointer pointer )
{
    gpointer result = g_object_get_data( ( GObject* ) pointer, "THIS" );
    
    if( result == NULL )
        return FALSE;
    
    MainWindow* dialog = reinterpret_cast< MainWindow* >( result );
    bool isChecked = gtk_check_menu_item_get_active( GTK_CHECK_MENU_ITEM( dialog->_lightingCheckButton ) );
    
    MeshCompletionApplication::getInstance()->setLightingEnabled( isChecked );
    
    return TRUE;
}

gboolean MainWindow::onWireframeButtonClicked( GtkWidget* button, gpointer pointer )
{
    gpointer result = g_object_get_data( ( GObject* ) pointer, "THIS" );
    
    if( result == NULL )
        return FALSE;
    
    MainWindow* dialog = reinterpret_cast< MainWindow* >( result );
    bool isChecked = gtk_check_menu_item_get_active( GTK_CHECK_MENU_ITEM( dialog->_wireframeCheckButton ) );
    
    MeshCompletionApplication::getInstance()->setWireframeEnabled( isChecked );
    
    return TRUE;
}

gboolean MainWindow::onFairingModeButtonClicked( GtkWidget* button, gpointer pointer )
{
    gpointer result = g_object_get_data( ( GObject* ) pointer, "THIS" );
    
    if( result == NULL )
        return FALSE;
    
    MainWindow* dialog = reinterpret_cast< MainWindow* >( result );
    
    MeshCompletionApplication::FairingMode mode;
    
    if( button == dialog->_scaleFairingButton )
        mode = MeshCompletionApplication::SCALAR;
    else if( button == dialog->_harmonicFairingButton )
        mode = MeshCompletionApplication::HARMONIC;
    else if( button == dialog->_secondOrderFairingButton )
        mode = MeshCompletionApplication::SECOND_ORDER;
    else
        mode = MeshCompletionApplication::NONE;
    
    MeshCompletionApplication::getInstance()->setFairingMode( mode );
    
    return TRUE;
}