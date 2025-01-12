#include "file-dialog.h"
#include "backend.h"
#include "colors.h"
#include "widgets.h"

static void handle_ok(UIWidget* w) {
	assert(w->parent);
	ui_file_dialog_close(w->parent);
}

static void handle_cancel(UIWidget* w) {
	assert(w->parent);
	ui_file_dialog_close(w->parent);
}

static void handle_list_double_click(UIWidget* w) {
	UIList* l = (UIList*)w;
	printf("DOUBLECLICK\n");
	if (l->selected_index) {
		UIFileDialog* fd = (UIFileDialog*)l->parent;
		assert(fd);
		char* selection = l->items[l->selected_index];
		printf("SELECTION: %s\n", selection);
		size_t path_length      = strlen(fd->path);
		size_t selection_length = strlen(selection);
		size_t length = strlen(fd->path) + 1 + strlen(selection);
		printf("LENGT %lu\n", length);

		fd->path = (char*)realloc(fd->path, sizeof(char) * length);
		assert(fd->path != NULL);
		printf("LENGT %lu\n", strlen(fd->path));
		//strcat(fd->path, fd->path);
		strcat(fd->path, PATH_SEPARATOR);
		strcat(fd->path, selection);
		printf("NEW: %s\n", fd->path);
		ui_file_dialog_scan(fd, fd->path);
	}
	//ui_file_dialog_scan((UIFileDialog*)l->parent, l->)
}

static void ui_file_dialog_clear(UIFileDialog* fd) {
	for (int i = 0; i < fd->list->items_count; i++) {
		destroy(fd->list->items[i]);
	}
}

UIWidget* ui_file_dialog(UIFileDialog args) {
	UIFileDialog* fd = new(UIFileDialog);
	UIWidget* widget = (UIWidget*)fd;

	memcpy(fd, &args, sizeof(UIFileDialog));

	UIList* list = new(UIList);
	list->children       = NULL;
	list->children_count = 0;
	list->double_click   = handle_list_double_click;
	list->items          = NULL;
	list->items_count    = 0;
	list->parent         = widget;
	list->selected_index = -1;
	ui_list(list);

	UIButton* cancel = new(UIButton);
	cancel->children       = NULL;
	cancel->children_count = 0;
	cancel->click  = handle_cancel;
	cancel->parent = widget;
	cancel->text   = "Cancel";
	ui_button(cancel);

	UIButton* ok = new(UIButton);
	ok->children       = NULL;
	ok->children_count = 0;
	ok->click  = handle_ok;
	ok->parent = widget;
	ok->text   = "OK";
	ui_button(ok);

	fd->list = list;
	fd->buttonOK = ok;
	fd->buttonCancel = cancel;

	UIWidget** children = malloc(sizeof(UIWidget*) * 3);
	children[0] = (UIWidget*)fd->list;
	children[1] = (UIWidget*)fd->buttonOK;
	children[2] = (UIWidget*)fd->buttonCancel;

	set_default(args.path, ".");
	fd->path = malloc(sizeof(char) * strlen(args.path));
	memcpy(fd->path, args.path, strlen(args.path));

	fd->state          = WIDGET_STATE_HIDDEN;
	fd->children       = children;
	fd->children_count = 3;
	fd->draw           = ui_file_dialog_draw;
	fd->type           = WIDGET_FILE_DIALOG;

	return widget;
}

void ui_file_dialog_close(UIWidget* w) {
	flag_on(w->state, WIDGET_STATE_HIDDEN);
	UIFileDialog* fd = (UIFileDialog*)w;
	//char* data       = ui_list_get_selected((UIWidget*)fd->list);
	assert(fd->close);
	fd->close(w, NULL);
}

void ui_file_dialog_destroy(UIWidget* w) {
	UIFileDialog* fd = (UIFileDialog*)w;
	ui_file_dialog_clear(fd);
	destroy(fd->list->items);
	destroy(fd->list);
	destroy(fd->buttonCancel);
	destroy(fd->buttonOK);
	destroy(fd->children);
	destroy(fd->path);
	destroy(fd);
}

void ui_file_dialog_draw(UIWidget* w, UIContext* c) {
	UIFileDialog* fd = (UIFileDialog*)w;

	ui_draw_rectangle(c, &(UIRectangleProperties){
		.color    = COLOR_DARK[0],
		.position = w->position,
		.size     = w->size
	});

	UIList* list       = (UIList*)fd->list;
	list->position     = (UIPosition){0, 30};
	list->size         = (UISize){w->size.width, w->size.height - 70};

	UIButton* ok       = (UIButton*)fd->buttonOK;
	ok->position.x     = w->size.width - 10 - ok->size.width;
	ok->position.y     = w->size.height - 10 - ok->size.height;

	UIButton* cancel   = (UIButton*)fd->buttonCancel;
	cancel->color      = COLOR_DARK;
	cancel->position.x = w->size.width - 20 - cancel->size.width - ok->size.width;
	cancel->position.y = w->size.height - 10 - cancel->size.height;

	list->draw((UIWidget*)list, c);
	cancel->draw((UIWidget*)cancel, c);
	ok->draw((UIWidget*)ok, c);
}

void ui_file_dialog_scan(UIFileDialog* fd, char* path) {
	if (fd->entries != NULL) {
		free(fd->entries);
		//ui_file_dialog_clear(fd);
	}
	KitFilesList* entries = new(KitFilesList);
	kit_folder_scan(path, entries);
	fd->entries = entries->items;
	fd->list->items = entries->items;
	fd->list->items_count = entries->count;
	fd->list->offset_y = 0;
}

void ui_file_dialog_show(UIWidget* w) {
	UIFileDialog* fd = (UIFileDialog*)w;
	flag_off(w->state, WIDGET_STATE_HIDDEN);
	ui_file_dialog_scan((UIFileDialog*)w, fd->path);
}
