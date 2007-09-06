/*
 * Copyright (C) 2007 Zeeshan Ali <zeenix@gstreamer.net>
 *
 * Authors: Zeeshan Ali <zeenix@gstreamer.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <string.h>
#include <stdlib.h>
#include <config.h>

#include "universal-cp-gui.h"
#include "universal-cp-devicetreeview.h"
#include "universal-cp-eventtreeview.h"
#include "universal-cp-actiondialog.h"
#include "universal-cp-icons.h"
#include "universal-cp.h"

#define GLADE_FILE "gupnp-universal-cp.glade"

GladeXML *glade_xml;

void
setup_treeview (GtkWidget    *treeview,
                GtkTreeModel *model,
                char         *headers[],
                int           render_index)
{
        int i;

        for (i = 0; headers[i] != NULL; i++) {
                GtkCellRenderer   *renderer;
                GtkTreeViewColumn *column;

                column = gtk_tree_view_column_new ();
                renderer = gtk_cell_renderer_text_new ();
                gtk_tree_view_column_pack_end (column, renderer, FALSE);
                gtk_tree_view_column_set_title (column, headers[i]);
                gtk_tree_view_column_add_attribute (column,
                                                    renderer,
                                                    "text", i + render_index);
                gtk_tree_view_column_set_sizing(column,
                                                GTK_TREE_VIEW_COLUMN_AUTOSIZE);

                gtk_tree_view_insert_column (GTK_TREE_VIEW (treeview),
                                             column,
                                             -1);
        }

        gtk_tree_view_set_model (GTK_TREE_VIEW (treeview),
                                 model);
}

static void
setup_treeviews (void)
{
        GtkWidget        *treeviews[3];
        GtkTreeModel     *treemodels[3];
        int               i;
        char             *headers[3][6] = { {"Name",
                                         "Value",
                                         NULL },
                                        {"Time",
                                         "Device",
                                         "Service",
                                         "State Variable",
                                         "Value",
                                         NULL } ,
                                        {"Device",
                                         NULL} };

        treeviews[0] = glade_xml_get_widget (glade_xml,
                        "details-treeview");
        treeviews[1] = glade_xml_get_widget (glade_xml,
                        "event-treeview");
        treeviews[2] = glade_xml_get_widget (glade_xml,
                        "device-treeview");
        treemodels[0] = create_details_treemodel ();
        treemodels[1] = create_event_treemodel ();
        treemodels[2] = create_device_treemodel ();
        g_assert (treeviews[0] != NULL &&
                  treeviews[1] != NULL &&
                  treeviews[2] != NULL);

        for (i = 0; i < 2; i++)
                setup_treeview (treeviews[i], treemodels[i], headers[i], 0);

        setup_device_treeview (treeviews[2], treemodels[2], headers[2], 1);
}

gboolean
on_delete_event (GtkWidget *widget,
                 GdkEvent  *event,
                 gpointer   user_data)
{
        gtk_main_quit ();
        return TRUE;
}

gboolean
init_ui (gint   *argc,
         gchar **argv[])
{
        GtkWidget *main_window;
        GtkWidget *hpaned;
        GtkWidget *vpaned;
        gint       window_width, window_height;
        gchar     *glade_path = NULL;
        gint       position;

        gtk_init (argc, argv);
        glade_init ();
        g_thread_init (NULL);

        /* Try to fetch the glade file from the CWD first */
        glade_path = GLADE_FILE;
        if (!g_file_test (glade_path, G_FILE_TEST_EXISTS)) {
                /* Then Try to fetch it from the system path */
                glade_path = DATA_DIR "/" GLADE_FILE;

                if (!g_file_test (glade_path, G_FILE_TEST_EXISTS))
                        glade_path = NULL;
        }

        if (glade_path == NULL) {
                g_critical ("Unable to load the GUI file %s", GLADE_FILE);
                return FALSE;
        }

        glade_xml = glade_xml_new (glade_path, NULL, NULL);
        if (glade_xml == NULL)
                return FALSE;

        main_window = glade_xml_get_widget (glade_xml, "main-window");
        hpaned = glade_xml_get_widget (glade_xml, "main-window-hpaned");
        vpaned = glade_xml_get_widget (glade_xml, "main-window-vpaned");
        g_assert (main_window != NULL);
        g_assert (hpaned != NULL);
        g_assert (vpaned != NULL);

        /* 80% of the screen but don't get bigger than 1000x800 */
        window_width = CLAMP ((gdk_screen_width () * 80 / 100), 10, 1000);
        window_height = CLAMP ((gdk_screen_height () * 80 / 100), 10, 800);
        gtk_window_set_default_size (GTK_WINDOW (main_window),
                                     window_width,
                                     window_height);

        glade_xml_signal_autoconnect (glade_xml);

        init_icons (glade_xml);
        setup_treeviews ();
        init_action_dialog ();

        gtk_widget_show_all (main_window);

        /* Give device treeview 40% of the main window */
        g_object_get (G_OBJECT (hpaned), "max-position", &position, NULL);
        position = position * 40 / 100;
        g_object_set (G_OBJECT (hpaned), "position", position, NULL);

        /* Give details treeview 60% of 60% of the main window */
        g_object_get (G_OBJECT (vpaned), "max-position", &position, NULL);
        position = position * 60 / 100;
        g_object_set (G_OBJECT (vpaned), "position", position, NULL);

        expanded = FALSE;

        return TRUE;
}

void
deinit_ui (void)
{
        deinit_icons ();
        g_object_unref (glade_xml);
}
