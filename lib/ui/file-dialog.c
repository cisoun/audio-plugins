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

UIWidget* ui_file_dialog(UIFileDialog args) {
	UIFileDialog* fd = new(UIFileDialog);
	UIWidget* widget = (UIWidget*)fd;

	memcpy(fd, &args, sizeof(UIFileDialog));

	char** files = malloc(sizeof(char*) * 2);
	files[0] = "Hello";
	files[1] = "World";

	fd->list = (UIList) {
		.items = files,
		.items_count = 2
	};

	fd->buttonCancel = (UIButton){
		.click  = handle_cancel,
		.parent = widget,
		.text   = "Cancel"
	};

	fd->buttonOK = (UIButton){
		.click  = handle_ok,
		.parent = widget,
		.text   = "OK"
	};

	ui_list(&fd->list);
	ui_button(&fd->buttonCancel);
	ui_button(&fd->buttonOK);

	UIWidget** items = malloc(sizeof(UIWidget*) * 2);
	items[0] = (UIWidget*)&fd->buttonOK;
	items[1] = (UIWidget*)&fd->buttonCancel;

	set_default(fd->state, WIDGET_STATE_HIDDEN);
	fd->children       = items;
	fd->children_count = 2;
	fd->draw           = ui_file_dialog_draw;
	fd->type           = WIDGET_FILE_DIALOG;
	return widget;
}

void ui_file_dialog_close(UIWidget* w) {
	flag_on(w->state, WIDGET_STATE_HIDDEN);
}

void ui_file_dialog_destroy(UIWidget* w) {
	UIFileDialog* fd = (UIFileDialog*)w;
	destroy(fd->children);
	destroy(fd);
}

void ui_file_dialog_draw(UIWidget* w, UIContext* c) {
	UIFileDialog* fd = (UIFileDialog*)w;
	ui_draw_rectangle(c, &(UIRectangleProperties){
		.color    = COLOR_DARK[0],
		.position = w->position,
		.size     = w->size
	});

	UIList* list       = (UIList*)&fd->list;
	list->position     = (UIPosition){10, 10};
	list->size         = (UISize){w->size.width - 20, w->size.height - 50};

	UIButton* ok       = (UIButton*)&fd->buttonOK;
	ok->position.x     = w->size.width - 10 - ok->size.width;
	ok->position.y     = w->size.height - 10 - ok->size.height;

	UIButton* cancel   = (UIButton*)&fd->buttonCancel;
	cancel->color      = COLOR_DARK;
	cancel->position.x = w->size.width - 20 - cancel->size.width - ok->size.width;
	cancel->position.y = w->size.height - 10 - cancel->size.height;

	ui_list_draw((UIWidget*)list, c);
	ui_button_draw((UIWidget*)cancel, c);
	ui_button_draw((UIWidget*)ok, c);
}

void ui_file_dialog_show(UIWidget* w) {
	flag_off(w->state, WIDGET_STATE_HIDDEN);
}
