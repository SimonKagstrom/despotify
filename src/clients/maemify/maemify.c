/*
 * DISCLAIMER:
 * The author is not a C programmer, and certainly not a gtk programmer either.
 * Don't expect good quality here.
 * 
 * This client was created as a proof of concept for the Nokia Maemo platform.
 * It should work with non-maemo GTK as well.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <pthread.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <wchar.h>

#ifdef MAEMO4
#include <hildon/hildon-window.h>
#include <hildon/hildon-program.h>
#include <hildon/hildon-find-toolbar.h>
#endif
#include <gtk/gtk.h>
#include <gtk/gtkmain.h>
#include <gdk/gdkkeysyms.h>

#include "despotify.h"


/* Application UI data struct */
typedef struct _AppData AppData;
struct _AppData {

    /* View items */
    #ifdef MAEMO4
     HildonProgram *program;
     HildonWindow *window;
    #else
     GtkWidget *window;
    #endif

    /* Toolbar */
    //GtkWidget *main_toolbar;

#ifdef MAEMO4
    /* Find toolbar */
    HildonFindToolbar *find_toolbar;
#else
    GtkWidget* find_toolbar;
    GtkWidget* table;
    GtkEntry* entry;
#endif

    /* Is Find toolbar visible or not */
    gboolean find_visible;

    /* Despotify playlists etc */
    struct despotify_session* ds;
    struct playlist* pl;
    struct playlist* rootlist;
    struct playlist* searchlist;
    struct playlist* lastlist;
    struct search_result *search;
    struct album_browse* playalbum;

    GtkWidget *list;

    GtkWidget *vbox;
    GtkWidget *label;
    GtkTreeSelection *selection;

};


enum
{
  LIST_ARTIST,
  LIST_TITLE,
  LIST_TRACK_POINTER,
  N_COLUMNS
};

/* these are global to allow the callback to access them */
static struct playlist* lastlist = NULL;
static int listoffset = 0;

/* Forward declarations */
static void init_list(GtkWidget *list);
static void add_to_list(GtkWidget *list, const gchar* str1, const gchar* str2, struct track *tracks);
void on_changed(GtkWidget *widget, AppData * appdata);
void tb_close_cb(GtkToolButton * widget, AppData * view);
void tb_find_cb(GtkToolButton * widget, AppData * view);
void find_tb_close(GtkWidget * widget, AppData * view);
void find_tb_search(GtkWidget * widget, AppData * view);
void create_find_toolbar(AppData * view);
void item_close_cb();
#ifdef MAEMO4
static void create_menu(HildonWindow * main_window);
#endif
bool despotify_deinit(AppData* appdata);
bool on_insert_text(GtkEntry* entry, GdkEventKey *event, AppData* appdata);
void do_search(gchar* searchstring, AppData* appdata);
void callback(int signal, void* data);

int main(int argc, char *argv[])
{
    if (argc < 3) {
        printf("Usage: %s <username> <password>\n", argv[0]);
        return 1;
    }

    GtkWidget *vbox;
    GtkWidget *swin;
    GtkTreeSelection *selection;


    /* Create needed variables - not anymore, use appdata instead
    HildonProgram *program;
    HildonWindow *window; */

    /* Create AppData */
    AppData *appdata=g_malloc( sizeof( *appdata ) );

    /* Initialize the GTK. */
    gtk_init(&argc, &argv);

    /* Create the Hildon program and setup the title */
    g_set_application_name("Maemify");
#ifdef MAEMO4
    appdata->program = HILDON_PROGRAM(hildon_program_get_instance());

    /* Create HildonWindow and set it to HildonProgram */
    appdata->window = HILDON_WINDOW(hildon_window_new());
    hildon_program_add_window(appdata->program, appdata->window);
#else
    appdata->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (appdata->window), "Maemify");
    gtk_widget_set_usize( GTK_WIDGET ( appdata->window ) , 600 , 300 );
    /* create our table */
    appdata->table = gtk_table_new(3, 3, FALSE); //three rows, three columns, not homogenous
    gtk_container_add(GTK_CONTAINER(appdata->window),appdata->table);
#endif


    /* Create find toolbar, but keep it hidden */
    create_find_toolbar(appdata);

#ifdef MAEMO4
    gtk_widget_show_all(GTK_WIDGET(appdata->find_toolbar));
    /* Create menu for the Window */
    create_menu(appdata->window);
#else
    gtk_table_attach_defaults(GTK_TABLE(appdata->table), GTK_WIDGET(appdata->entry), 1, 2, 2, 3);
    gtk_widget_show_all(GTK_WIDGET(appdata->table));
#endif

    /* Begin the main application */
    gtk_widget_show_all(GTK_WIDGET(appdata->window));

    /* Connect signal to X in the upper corner */
    g_signal_connect(G_OBJECT(appdata->window), "delete_event",
    G_CALLBACK(gtk_main_quit), NULL);
    if (!despotify_init())
    {
        printf("despotify_init() failed\n");
        return 1;
    }

    appdata->ds = despotify_init_client(callback);
    if (!appdata->ds) {
        printf("despotify_init_client() failed\n");
        return 1;
    }
    if (!despotify_authenticate(appdata->ds, argv[1], argv[2])) {
        printf( "Authentication failed: %s\n", despotify_get_error(appdata->ds));
        despotify_exit(appdata->ds);
        return 1;
    }

  /* Create a tree view and place it in a scrolled window */
  appdata->list = gtk_tree_view_new();
  gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(appdata->list), TRUE);
  swin = gtk_scrolled_window_new(NULL, NULL);
  
  vbox = gtk_vbox_new(FALSE, 0);

  /* add labels to the fields */
  appdata->label = gtk_label_new("Search hits");
  gtk_label_set_justify(GTK_LABEL(appdata->label), GTK_JUSTIFY_CENTER);
  gtk_box_pack_start(GTK_BOX(vbox), appdata->label, FALSE, FALSE, 5);

  gtk_container_add(GTK_CONTAINER(swin), appdata->list);
#ifdef MAEMO4
  gtk_container_add(GTK_CONTAINER(appdata->window), swin);
#else
  gtk_table_attach_defaults(GTK_TABLE(appdata->table), swin, 1, 2, 1, 2);
#endif

  /* initialize a list to hold search hits */
  init_list(appdata->list);

  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(appdata->list));

  g_signal_connect(selection, "changed", 
      G_CALLBACK(on_changed), appdata);

    gtk_widget_show_all(GTK_WIDGET(appdata->window));

    gtk_main();

    despotify_deinit(appdata);

    /* Exit */
    return 0;
}











/* 
 * List initializer.
 * Creates an actual list with it's columns and datastore
 */
static void 
init_list(GtkWidget *list)
{

   GtkCellRenderer *renderer;
   GtkTreeViewColumn *column;
   GtkListStore *store;

   /* create artist column */
   renderer = gtk_cell_renderer_text_new();
   column = gtk_tree_view_column_new_with_attributes("Artist",
          renderer, "text", LIST_ARTIST, NULL);
   gtk_tree_view_column_set_resizable (column, TRUE);
   gtk_tree_view_append_column(GTK_TREE_VIEW(list), column);

   /* create title column */
   renderer = gtk_cell_renderer_text_new();
   column = gtk_tree_view_column_new_with_attributes("Title",
          renderer, "text", LIST_TITLE, NULL);
   gtk_tree_view_column_set_resizable (column, TRUE);
   gtk_tree_view_append_column(GTK_TREE_VIEW(list), column);

   /* Create a data store associated with each row */
   store = gtk_list_store_new(N_COLUMNS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_POINTER);

   gtk_tree_view_set_model(GTK_TREE_VIEW(list), 
       GTK_TREE_MODEL(store));

   g_object_unref(store);
}

/* 
 * List item adding function.
 * Adds a (track) item to a list
 */
static void 
add_to_list(GtkWidget *list, const gchar* col1str, const gchar* col2str, struct track *track)
{
  GtkListStore *store;
  GtkTreeIter iter;

  store = GTK_LIST_STORE(gtk_tree_view_get_model
      (GTK_TREE_VIEW(list)));

  gtk_list_store_append(store, &iter);
  gtk_list_store_set(store, &iter, LIST_ARTIST, col1str, LIST_TITLE, col2str, LIST_TRACK_POINTER, track, -1);
}

/* 
 * Handle list item clicking
 * At the moment the clicked line is just played back.
 */
void on_changed(GtkWidget *widget, AppData * appdata) 
{
  GtkTreeIter iter;
  GtkTreeModel *model;
  char *artist;
  char *title;
  struct track *track;


  if (gtk_tree_selection_get_selected(
      GTK_TREE_SELECTION(widget), &model, &iter)) {

    gtk_tree_model_get(model, &iter, LIST_ARTIST, &artist, LIST_TITLE, &title, LIST_TRACK_POINTER, &track, -1);
    gtk_label_set_text(GTK_LABEL(appdata->label), artist);
    /* play this item. The list it belongs to will be the playlist */
    despotify_play(appdata->ds, track, true);

    g_free(artist);
    g_free(title);
  }

}


/* 
 * Callback for "Close" toolbar button 
 */
void tb_close_cb(GtkToolButton * widget, AppData * view)
{
    //currently this is disabled
    return;
    gtk_widget_show ( GTK_WIDGET(widget) );
    gtk_widget_hide_all(GTK_WIDGET(view->find_toolbar));
    view->find_visible = FALSE;
}

/* 
 * Callback for "Close" find toolbar button 
 */
void find_tb_close(GtkWidget * widget, AppData * view)
{
    //currently this is disabled
    return;
    gtk_widget_show ( GTK_WIDGET(widget) );
    gtk_widget_hide_all(GTK_WIDGET(view->find_toolbar));
    view->find_visible = FALSE;
}

/*
 * Callback for "Search" find toolbar button
 */
void find_tb_search(GtkWidget * widget, AppData * appdata)
{
    gchar *find_text = NULL;
    g_object_get(G_OBJECT(widget), "prefix", &find_text, NULL);

    do_search(find_text, appdata);

    g_free(find_text);
}

/*
 * Search for a string on the server.
 * Results are added to the default search list.
 */
void do_search(gchar* find_text, AppData* appdata) {
  GtkListStore *store;
  GtkTreeIter iter;

   if(appdata->ds) {

      //first clean out old search results
        store = GTK_LIST_STORE(gtk_tree_view_get_model
            (GTK_TREE_VIEW(appdata->list)));
        if(gtk_tree_model_get_iter_first(GTK_TREE_MODEL(store), &iter))
           while(gtk_list_store_remove(store, &iter));
        

      appdata->search = despotify_search(appdata->ds, find_text, MAX_SEARCH_RESULTS);
      if (!appdata->search) {
          printf("Search failed: %s\n", despotify_get_error(appdata->ds));
      }
      else {
         appdata->searchlist = appdata->search->playlist;
         int i=1;
         struct track* t = appdata->searchlist->tracks;
         struct track* t_last;
         for (; t; t = t->next) {
             printf("%2d: %s (%d:%02d), ptr: %d\n", i++, t->title,
                t->length / 60000, t->length % 60000 / 1000, (int)t);
             add_to_list(appdata->list, t->artist->name, t->title, t);
             t_last=t;
         }
      }
      //FIXME: move to deinit or new search. Perhaps done at each new search?
      //despotify_free_playlist(appdata->search);
   }
}





/* 
 * Create the find toolbar
*/
void create_find_toolbar(AppData * view)
{
#ifdef MAEMO4
    view->find_toolbar = HILDON_FIND_TOOLBAR
        (hildon_find_toolbar_new("Find String: "));

    /* Add signal listers to "Search" and "Close" buttons */
    g_signal_connect(G_OBJECT(view->find_toolbar), "search",
                     G_CALLBACK(find_tb_search), view);
    g_signal_connect(G_OBJECT(view->find_toolbar), "close",
                     G_CALLBACK(find_tb_close), view);
    hildon_window_add_toolbar(view->window, GTK_TOOLBAR(view->find_toolbar));

    /* Set variables to AppData */
    view->find_visible = FALSE;
#else
    view->find_toolbar = gtk_toolbar_new();
    view->entry = GTK_ENTRY(gtk_entry_new ());
  
    g_signal_connect(GTK_WIDGET(view->entry), "key-press-event",
                   G_CALLBACK(on_insert_text), (gpointer)view);

    gtk_widget_show ( GTK_WIDGET(view->entry) );
#endif

}


/* 
 * Callback for find text insertion in non-maemo context
*/
bool on_insert_text(GtkEntry* entry, GdkEventKey *event, AppData* appdata)
{

   gchar* find_text = gtk_editable_get_chars(GTK_EDITABLE(entry), 0, -1);
   if(event->keyval == GDK_Return)
      do_search(find_text, appdata);

   g_free(find_text);
return FALSE;
}

/* 
 * Callback for "Close" menu entry
*/
void item_close_cb()
{
    g_print("Closing application...\n");
    gtk_main_quit();
}

bool 
despotify_deinit(AppData* appdata)
{
    if(appdata->ds) despotify_exit(appdata->ds);

    if (!despotify_cleanup())
    {
        printf("despotify_cleanup() failed\n");
        return 1;
    }
    return 0;
}

#ifdef MAEMO4
/* 
 * Create the menu items needed for the main view.
 * This needs to be implemented properly.
*/
static void create_menu(HildonWindow * main_window) 
{
    /* Create needed variables */
    GtkWidget *main_menu;
    GtkWidget *menu_others;
    GtkWidget *item_others;
    GtkWidget *item_radio1;
    GtkWidget *item_radio2;
    GtkWidget *item_check;
    GtkWidget *item_close;
    GtkWidget *item_separator;

    /* Create new main menu */
    main_menu = gtk_menu_new();

    /* Create new submenu for "Others" */
    menu_others = gtk_menu_new();

    /* Create menu items */
    item_others = gtk_menu_item_new_with_label("Others");
    item_radio1 = gtk_radio_menu_item_new_with_label(NULL, "Radio1");
    item_radio2 =
        gtk_radio_menu_item_new_with_label_from_widget(GTK_RADIO_MENU_ITEM
                                                       (item_radio1),
                                                       "Radio2");
    item_check = gtk_check_menu_item_new_with_label("Check");
    item_close = gtk_menu_item_new_with_label("Close");
    item_separator = gtk_separator_menu_item_new();

    /* Add menu items to right menus */
    gtk_menu_append(main_menu, item_others);
    gtk_menu_append(menu_others, item_radio1);
    gtk_menu_append(menu_others, item_radio2);
    gtk_menu_append(menu_others, item_separator);
    gtk_menu_append(menu_others, item_check);
    gtk_menu_append(main_menu, item_close);

    /* Add others submenu to the "Others" item */
    hildon_window_set_menu(HILDON_WINDOW(main_window), GTK_MENU(main_menu));
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(item_others), menu_others);

    /* Attach the callback functions to the activate signal */
    g_signal_connect(G_OBJECT(item_close), "activate",
                     GTK_SIGNAL_FUNC(item_close_cb), NULL);

    /* Make all menu widgets visible */
    gtk_widget_show_all(GTK_WIDGET(main_menu));
}
#endif


void callback(int signal, void* data)
{
    (void)data;

    switch (signal) {
        case DESPOTIFY_TRACK_CHANGE:
            listoffset++;
            struct track* t = lastlist->tracks;
            for (int i=1; i<listoffset && t; i++)
                t = t->next;
            if (t)
                wprintf(L"New track: %d: %s / %s (%d:%02d)\n",
                        listoffset, t->title, t->artist->name,
                        t->length / 60000, t->length % 60000 / 1000);
            break;
    }
}

