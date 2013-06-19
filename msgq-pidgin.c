#define PURPLE_PLUGINS

#define VERSION "0.0.2"
#define PLUGIN_ID "gtk-msgq-comnot"
#define PLUGIN_NAME "msgq-pidgin"

#include <glib.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "notify.h"
#include "plugin.h"
#include "version.h"
#include "debug.h"
#include "cmds.h"
#include "gtkconv.h"
#include "prefs.h"
#include "gtkprefs.h"
#include "gtkutils.h"
#include "gtkplugin.h"
#include "gtkblist.h"

#define DEFAULT_ADDR "127.0.0.1"
#define DEFAULT_PORT 1024
#define DEFAULT_MOD "*"
#define DEFAULT_TOPIC "*"

static char *msg_tmpl = "{\"msg\":\"%s\", \"mod\":\"%s\", \"topic\":\"%s\"}";

static int sockfd_create(char *addr, int port)
{
    struct sockaddr_in servaddr;
    int sockfd;

    if (addr == NULL)
        addr = DEFAULT_ADDR;
    if (port == 0)
        port = DEFAULT_PORT;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    inet_pton(AF_INET, addr, &servaddr.sin_addr);
    servaddr.sin_port = htons(port);
    connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

    return sockfd;
}

static void sockfd_close(int sockfd)
{
    close(sockfd);
}

static char *msg_create(const char *msg, const char *topic, const char *mod)
{
    char *buf;

    if (topic == NULL)
        topic = DEFAULT_TOPIC;
    if (mod == NULL)
        mod = DEFAULT_MOD;
    buf = malloc(sizeof(char) * (strlen(msg_tmpl) + strlen(msg) + strlen(mod)\
                                 +strlen(topic) + 1 - 6));
    sprintf(buf, msg_tmpl, msg, mod, topic);
    return buf;
}

static void msg_destory(char *buf)
{
    free(buf);
}

static void msg_send(char *addr, int port, const char *msg,
                     const char *topic, const char *mod)
{
    int fd;
    char *json;

    fd = sockfd_create(addr, port);
    json = msg_create(msg, topic, mod);
    write(fd, json, strlen(json));
    msg_destory(json);
    sockfd_close(fd);
}

static gboolean purple_status_is_avaliable(PurpleStatus *status)
{
    PurpleStatusPrimitive primitive = PURPLE_STATUS_UNSET;

    if (!status)
        return FALSE;

    primitive = purple_status_type_get_primitive(purple_status_get_type(status));

    return (primitive == PURPLE_STATUS_AVAILABLE);
}

static void msg_received(PurpleAccount *account, char *sender, char *message,
                         PurpleConversation *conv, PurpleMessageFlags flags)
{
    char *topic, *mod, *host, *_port;
    int port;
    PurpleStatus *status;

    topic = (char *) purple_prefs_get_string("/plugins/gtk/"PLUGIN_ID"/topic");
    mod = (char *) purple_prefs_get_string("/plugins/gtk/"PLUGIN_ID"/mod");
    host = (char *) purple_prefs_get_string("/plugins/gtk/"PLUGIN_ID"/host");
    _port = (char *) purple_prefs_get_string("/plugins/gtk/"PLUGIN_ID"/port");
    port = atoi(_port);

    purple_debug_info(PLUGIN_NAME, sender);
    purple_debug_info(PLUGIN_NAME, message);
    purple_debug_info(PLUGIN_NAME, topic);
    purple_debug_info(PLUGIN_NAME, mod);

    status = purple_account_get_active_status(account);
    if (status != NULL && !purple_status_is_avaliable(status)) {
        msg_send(host, port, (const char *) message, topic, mod);
    }
}

static GtkWidget *plugin_config_frame(PurplePlugin *plugin)
{
    GtkWidget *frame;
    GtkWidget *vbox;
    GtkSizeGroup *sg;

    frame = gtk_vbox_new(FALSE, 18);
    gtk_container_set_border_width(GTK_CONTAINER(frame), 12);

    vbox = pidgin_make_frame(frame, "Server settings...");
    sg = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);
    pidgin_prefs_labeled_entry(vbox, "IP",
                               "/plugins/gtk/"PLUGIN_ID"/ip", sg);
    pidgin_prefs_labeled_entry(vbox, "Port",
                               "/plugins/gtk/"PLUGIN_ID"/port", sg);
    pidgin_prefs_labeled_entry(vbox, "Mod",
                               "/plugins/gtk/"PLUGIN_ID"/mod", sg);
    pidgin_prefs_labeled_entry(vbox, "Topic",
                               "/plugins/gtk/"PLUGIN_ID"/topic", sg);

    gtk_widget_show_all(frame);
    return frame;
}

static void init_plugin(PurplePlugin *plugin)
{
    /* load configuration view */
    purple_prefs_add_none("/plugins/gtk/"PLUGIN_ID);
    purple_prefs_add_string("/plugins/gtk/"PLUGIN_ID"/host", "127.0.0.1");
    purple_prefs_add_string("/plugins/gtk/"PLUGIN_ID"/port", "1024");
    purple_prefs_add_string("/plugins/gtk/"PLUGIN_ID"/topic", "received");
    purple_prefs_add_string("/plugins/gtk/"PLUGIN_ID"/mod", "message");
}

static gboolean plugin_load(PurplePlugin *plugin)
{
    purple_signal_connect(purple_conversations_get_handle(),
                          "received-im-msg", plugin,
                          PURPLE_CALLBACK(msg_received), NULL);
    return TRUE;
}

static gboolean plugin_unload(PurplePlugin *plugin)
{
    purple_signal_disconnect(purple_conversations_get_handle(),
                             "received-im-msg", plugin,
                             PURPLE_CALLBACK(msg_received));
    return TRUE;
}


static PidginPluginUiInfo ui_info = {
    plugin_config_frame,
    0
};


static PurplePluginInfo info = {
    PURPLE_PLUGIN_MAGIC,
    PURPLE_MAJOR_VERSION,
    PURPLE_MINOR_VERSION,
    PURPLE_PLUGIN_STANDARD,
    PIDGIN_PLUGIN_TYPE,
    0,
    NULL,
    PURPLE_PRIORITY_DEFAULT,

    PLUGIN_ID,
    PLUGIN_NAME,
    VERSION,

    "msgQ plugin for pidgin",
    "Send some message to the queue when new im message ping you up.",
    "hbc <bcxxxxxx@gmail.com>",
    "http://youknowmymind.com",

    plugin_load,   /* load */
    plugin_unload, /* unload */
    NULL,          /* destroy */

    &ui_info,
    NULL,
    NULL,
    NULL
};

PURPLE_INIT_PLUGIN(comnot, init_plugin, info);
