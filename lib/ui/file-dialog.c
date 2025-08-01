#include "file-dialog.h"

static void open_file(UIFileDialog* fd, KitFileInfo* kfi) {
	KitFileInfo* fi = ui_list_get_selection((UIWidget*)fd->list);
	if (fi != NULL) {
		ui_file_dialog_close((UIWidget*)fd);
		fd->close((UIWidget*)fd, fi);
	}
}

static inline void open_path(UIFileDialog* fd, KitFileInfo* kfi) {
	char* path = kit_string_join3(kfi->path, PATH_SEPARATOR, kfi->name);
	ui_file_dialog_scan(fd, path);
	destroy(path);
}

static void handle_ok(UIWidget* w) {
	UIFileDialog* fd  = (UIFileDialog*)w->parent;
	KitFileInfo*  kfi = ui_file_list_get_selection((UIWidget*)fd->list);
	if (kfi->type == KIT_FILE_TYPE_FOLDER) {
		open_path(fd, kfi);
	} else if (kfi->type == KIT_FILE_TYPE_FILE) { // && ui_file_list_is_valid(w, kfi)) {
		open_file(fd, kfi);
	}
}

static void handle_cancel(UIWidget* w) {
	ui_file_dialog_close(w->parent);
	UIFileDialog* fd = (UIFileDialog*)w->parent;
	fd->close(w, NULL);
}

static void handle_list_double_click(UIWidget* w) {
	UIFileList*   l     = (UIFileList*)w;
	UIFileDialog* fd    = (UIFileDialog*)l->parent;
	unsigned int  index = ui_list_get_selection_index(w);
	if (index > 0 && !fd->has_selection_changed) {
		KitFileInfo*  kfi = ui_file_list_get_selection(w);
		if (kfi->type == KIT_FILE_TYPE_FOLDER) {
			open_path(fd, kfi);
		} else {
			open_file(fd, kfi);
		}
	}
}

static void handle_selection_change(UIWidget* w, unsigned int index) {
	UIFileList*   fl = (UIFileList*)w;
	UIFileDialog* fd = (UIFileDialog*)fl->parent;
	if (index > 0) {
		ui_widget_enable((UIWidget*)fd->buttonOK);
	} else {
		ui_widget_disable((UIWidget*)fd->buttonOK);
	}
	fd->has_selection_changed = index != fd->selection_index;
	fd->selection_index       = index;
}

UIWidget* ui_file_dialog(UIFileDialog args) {
	UIFileDialog* fd     = new(UIFileDialog);
	UIWidget*     widget = (UIWidget*)fd;
	set_default(args.path, ".");
	memcpy(fd, &args, sizeof(UIFileDialog));

	UIText* text           = new(UIText);
	text->parent           = widget;
	text->position         = (UIPosition){10, 10};
	text->size             = (UISize){fd->size.width - 20, 20};
	ui_text(text);

	UIFileList* list       = new(UIFileList);
	list->double_click     = handle_list_double_click;
	list->filters          = fd->filters;
	list->items            = fd->files;
	list->parent           = widget;
	list->selection_change = handle_selection_change;
	ui_file_list(list);

	UIButton* cancel       = ui_button_new();
	cancel->click          = handle_cancel;
	cancel->parent         = widget;
	cancel->text           = "Cancel";

	UIButton* ok           = ui_button_new();
	ok->click              = handle_ok;
	ok->parent             = widget;
	ok->text               = "OK";

	fd->text               = text;
	fd->list               = list;
	fd->buttonOK           = ok;
	fd->buttonCancel       = cancel;

	UIWidget** children = alloc(UIWidget*, 4);
	children[0] = (UIWidget*)fd->list;
	children[1] = (UIWidget*)fd->buttonOK;
	children[2] = (UIWidget*)fd->buttonCancel;
	children[3] = (UIWidget*)fd->text;

	set_default(fd->filters, (char*[]){});
	fd->children       = children;
	fd->children_count = 4;
	fd->draw           = ui_file_dialog_draw;
	fd->files          = kit_array();
	fd->path           = kit_string_clone(args.path);
	fd->state          = WIDGET_STATE_HIDDEN;
	fd->type           = WIDGET_FILE_DIALOG;

	ui_file_dialog_scan(fd, fd->path);

	return widget;
}

inline void ui_file_dialog_close(UIWidget* w) {
	ui_widget_set_state(w, WIDGET_STATE_HIDDEN);
	ui_widget_must_redraw(w);
}

void ui_file_dialog_destroy(UIWidget* w) {
	UIFileDialog* fd = (UIFileDialog*)w;
	if (fd->files) {
		kit_file_info_array_destroy(fd->files);
	}
	ui_file_list_destroy(fd->list);
	ui_button_destroy(fd->buttonOK);
	ui_button_destroy(fd->buttonCancel);
	ui_text_destroy(fd->text);
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

void ui_file_dialog_scan(UIFileDialog* fd, const char* path) {
	if (fd->files != NULL) {
		kit_file_info_array_clear(fd->files);
	}
	char buffer[PATH_MAX];
	if (realpath(path, buffer)) {
		fd->files = kit_path_scan(buffer);
		if (fd->files == NULL) {
			fd->files = kit_array();
			KitFileInfo* fi = kit_file_info((KitFileInfo){
				.name = "..",
				.path = fd->path,
				.type = KIT_FILE_TYPE_FOLDER
			});
			kit_array_add(fd->files, fi);
		}
		char* new_path = kit_string_clone(buffer);
		destroy(fd->path);
		fd->path                 = new_path;
		fd->list->items          = fd->files;
		fd->list->offset_y       = 0;
		ui_text_set_text((UIWidget*)fd->text, fd->path);
		ui_list_select((UIWidget*)fd->list, 0);
	}
}

void ui_file_dialog_show(UIWidget* w) {
	ui_widget_unset_state(w, WIDGET_STATE_HIDDEN);
	ui_widget_must_redraw(w);
}
