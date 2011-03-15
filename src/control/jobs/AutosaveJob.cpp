#include "AutosaveJob.h"
#include "../SaveHandler.h"

#include "../Control.h"

AutosaveJob::AutosaveJob(Control * control) {
	this->control = control;
}

AutosaveJob::~AutosaveJob() {
}

void AutosaveJob::afterRun() {
	GtkWidget * dialog = gtk_message_dialog_new((GtkWindow *) control->getWindow(), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Autosave: %s"),
			this->error.c_str());
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}

void AutosaveJob::run() {
	SaveHandler handler;

	control->getUndoRedoHandler()->documentAutosaved();

	Document * doc = control->getDocument();

	doc->lock();
	handler.prepareSave(doc);
	String filename = doc->getFilename();
	doc->unlock();

	if (filename.isEmpty()) {
		filename = Util::getAutosaveFilename();
	} else {
		if (filename.length() > 5 && filename.substring(-4) == ".xoj") {
			filename = filename.substring(0, -4);
		}
		filename += ".autosave.xoj";
	}

	control->deleteLastAutosaveFile(filename);

	GzOutputStream * out = new GzOutputStream(filename);
	handler.saveTo(out, filename);
	delete out;

	this->error = handler.getErrorMessage();
	if (!this->error.isEmpty()) {
		callAfterRun();
	}
}

JobType AutosaveJob::getType() {
	return JOB_TYPE_AUTOSAVE;
}
