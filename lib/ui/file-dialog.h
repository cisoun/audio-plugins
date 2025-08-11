#ifndef UI_FILE_DIALOG_H
#define UI_FILE_DIALOG_H

#include "../kit.h"
#include "backend.h"
#include "colors.h"
#include "widgets.h"
#include <errno.h>
#include <string.h>

typedef struct {
	WIDGET
	UIButton*   buttonCancel;
	UIButton*   buttonOK;
	KitArray*   files;
	char**      filters;
	bool        has_selection_changed;
	UIFileList* list;
	char*       path;
	int         selection_index;
	UIText*     text;
	void        (*cancel) (UIWidget*);
	void        (*close)  (UIWidget*, KitFileInfo*);
} UIFileDialog;

UIWidget* ui_file_dialog         (UIFileDialog);
void      ui_file_dialog_close   (UIWidget*);
void      ui_file_dialog_destroy (UIWidget*);
void      ui_file_dialog_draw    (UIWidget*, UIContext*);
void      ui_file_dialog_scan    (UIFileDialog*, const char*);
void      ui_file_dialog_show    (UIWidget*);

#endif
