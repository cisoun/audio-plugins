#ifndef UI_FILE_DIALOG_H
#define UI_FILE_DIALOG_H

#include "backend.h"
#include "colors.h"
#include "widgets.h"

typedef struct {
	WIDGET
	UIButton*   buttonCancel;
	UIButton*   buttonOK;
	KitArray*   files;
	char**      filters;
	UIFileList* list;
	char*       path;
	UIText*     text;
	void        (*cancel) (UIWidget*);
	void        (*close)  (UIWidget*, char*);
} UIFileDialog;

UIWidget* ui_file_dialog         (UIFileDialog);
void      ui_file_dialog_close   (UIWidget*);
void      ui_file_dialog_destroy (UIWidget*);
void      ui_file_dialog_draw    (UIWidget*, UIContext*);
void      ui_file_dialog_scan    (UIFileDialog*, const char*);
void      ui_file_dialog_show    (UIWidget*);

#endif
