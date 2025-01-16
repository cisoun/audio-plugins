#include <limits.h>
#include <string.h>
#include <errno.h>
#include "backend.h"
#include "colors.h"
#include "file-dialog.h"
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
	UIFileList* l = (UIFileList*)w;
	if (l->selected_index > -1) {
		UIFileDialog* fd    = (UIFileDialog*)l->parent;
		KitArray*     items = l->items;
		KitFileInfo*  item  = items->items[l->selected_index];
		if (item->type == KIT_FILE_TYPE_FOLDER) {
			size_t l = strlen(item->path) + strlen(PATH_SEPARATOR) + strlen(item->name);
			char* buffer = alloc(char, l + 1);
			strcpy(buffer, item->path);
			strcat(buffer, PATH_SEPARATOR);
			strcat(buffer, item->name);
			destroy(fd->path);
			fd->path = buffer;
			ui_file_dialog_scan(fd, fd->path);
		}
	}
}

UIWidget* ui_file_dialog(UIFileDialog args) {
	UIFileDialog* fd     = new(UIFileDialog);
	UIWidget*     widget = (UIWidget*)fd;

	memcpy(fd, &args, sizeof(UIFileDialog));

	UIText* text   = new(UIText);
	text->position = (UIPosition){10, 10};
	ui_text(text);

	UIFileList* list       = new(UIFileList);
	list->double_click     = handle_list_double_click;
	list->filters          = fd->filters;
	list->items            = fd->files;
	list->parent           = widget;
	list->selected_index   = -1;
	ui_file_list(list);

	UIButton* cancel       = ui_button_new();
	cancel->click          = handle_cancel;
	cancel->parent         = widget;
	cancel->text           = "Cancel";

	UIButton* ok           = ui_button_new();
	ok->click              = handle_ok;
	ok->parent             = widget;
	ok->text               = "OK";

	fd->text         = text;
	fd->list         = list;
	fd->buttonOK     = ok;
	fd->buttonCancel = cancel;

	UIWidget** children = alloc(UIWidget*, 4);
	children[0] = (UIWidget*)fd->list;
	children[1] = (UIWidget*)fd->buttonOK;
	children[2] = (UIWidget*)fd->buttonCancel;
	children[3] = (UIWidget*)fd->text;

	set_default(args.path, ".");
	set_default(fd->filters, (char*[]){});
	fd->children       = children;
	fd->children_count = 4;
	fd->draw           = ui_file_dialog_draw;
	fd->files          = kit_array();
	fd->path           = kit_string_clone(args.path);
	fd->state          = WIDGET_STATE_HIDDEN;
	fd->type           = WIDGET_FILE_DIALOG;

	return widget;
}

void ui_file_dialog_close(UIWidget* w) {
	flag_on(w->state, WIDGET_STATE_HIDDEN);
	UIFileDialog* fd = (UIFileDialog*)w;
	assert(fd->close);
	fd->close(w, NULL);
}

void ui_file_dialog_destroy(UIWidget* w) {
	UIFileDialog* fd = (UIFileDialog*)w;
	if (fd->files) {
		kit_file_info_array_destroy(fd->files);
	}
	ui_file_list_destroy(fd->list);
	ui_button_destroy(fd->buttonOK);
	ui_button_destroy(fd->buttonCancel);
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

	UIFileList* list   = fd->list;
	list->position     = (UIPosition){0, 30};
	list->size         = (UISize){w->size.width, w->size.height - 70};

	UIButton* ok       = fd->buttonOK;
	ok->position.x     = w->size.width - 10 - ok->size.width;
	ok->position.y     = w->size.height - 10 - ok->size.height;

	UIButton* cancel   = fd->buttonCancel;
	cancel->color      = COLOR_DARK;
	cancel->position.x = w->size.width - 20 - cancel->size.width - ok->size.width;
	cancel->position.y = w->size.height - 10 - cancel->size.height;

	ok->draw((UIWidget*)ok, c);
	list->draw((UIWidget*)list, c);
	cancel->draw((UIWidget*)cancel, c);
}

void ui_file_dialog_scan(UIFileDialog* fd, char* path) {
	KitArray* files = fd->files;
	if (files != NULL) {
		kit_file_info_array_clear(files);
	}
	char buffer[PATH_MAX];
	if (realpath(fd->path, buffer)) {
		KitArray* entries        = kit_path_scan(buffer);
		destroy(fd->path);
		fd->path                 = kit_string_clone(buffer);
		fd->files                = entries;
		fd->list->items          = entries;
		fd->list->offset_y       = 0;
		fd->list->selected_index = -1;
		fd->text->text = fd->path;
	}
}

void ui_file_dialog_show(UIWidget* w) {
	UIFileDialog* fd = (UIFileDialog*)w;
	flag_off(w->state, WIDGET_STATE_HIDDEN);
	ui_file_dialog_scan(fd, fd->path);
}
