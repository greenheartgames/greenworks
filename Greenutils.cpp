/* INCLUDE HEADER */
#include "Greenutils.h"

/* ********************************************************************************************************************************** */
/* PRIVATE  MEMBERS */
/* ********************************************************************************************************************************** */
Greenutils::Greenutils() {};
Greenutils::~Greenutils() {};

Persistent<Function> Greenutils::constructor;

Handle<Value> Greenutils::New(const Arguments& args) {
	HandleScope scope;

	if (args.IsConstructCall()) {
		// Invoked as constructor: `new MyObject(...)`
		Greenutils* obj = new Greenutils();
		obj->Wrap(args.This());
		return args.This();
	}
	else {
		// Invoked as plain function `MyObject(...)`, turn into construct call.
		return scope.Close(constructor->NewInstance());
	}
}

void Greenutils::extractArchiveWorker(uv_work_t *req) {
	CreateExtractArchive *ea = (CreateExtractArchive*)req->data;

	CUnzip* uz = new CUnzip();
	int ret = uz->unzip(ea->targetFile.c_str(), ea->targetFolder.c_str(), ea->password.empty() ? NULL : ea->password.c_str());

	CUtils::Sleep(250);

	ea->error = "Error trying to unzip " + ea->targetFile + " to " + ea->targetFolder;

	ea->bSuccess = ret == 0;
}

void Greenutils::extractArchiveWorkerComplete(uv_work_t *req) {
	CreateExtractArchive *ea = (CreateExtractArchive*)req->data;

	// Calling callback here
	Handle<Value> callbackResultArgs[1];

	if (ea->bSuccess) {
		CUtils::WriteToJsConsoleAndLogFile("Archive successfully extracted.");

		callbackResultArgs[0] = Undefined();

		if (ea->successCallback->IsCallable()){
			ea->successCallback->Call(Context::GetCurrent()->Global(), 1, callbackResultArgs);
		}
	}
	else {
		CUtils::WriteToJsConsoleAndLogFile("Archive could not be extracted!");

		callbackResultArgs[0] = String::New(ea->error.c_str());
		if (ea->errorCallback->IsCallable()){
			ea->errorCallback->Call(Context::GetCurrent()->Global(), 1, callbackResultArgs);
		}
	}

	return;
}

void Greenutils::createArchiveWorker(uv_work_t *req) {
	CreateExtractArchive *ea = (CreateExtractArchive*)req->data;

	CZip* zz = new CZip();
	int ret = zz->zip(ea->targetFile.c_str(), ea->targetFolder.c_str(), ea->compressionLevel, ea->password.empty() ? NULL : ea->password.c_str());

	CUtils::Sleep(250);

	ea->error = "Error trying to create archive " + ea->targetFile + " from " + ea->targetFolder;

	ea->bSuccess = ret == 0;
}

void Greenutils::createArchiveWorkerComplete(uv_work_t *req) {
	CreateExtractArchive *ea = (CreateExtractArchive*)req->data;

	// Calling callback here
	Handle<Value> callbackResultArgs[1];

	if (ea->bSuccess) {
		CUtils::WriteToJsConsoleAndLogFile("Archive successfully created.");

		callbackResultArgs[0] = Undefined();

		if (ea->successCallback->IsCallable()){
			ea->successCallback->Call(Context::GetCurrent()->Global(), 1, callbackResultArgs);
		}
	}
	else {
		CUtils::WriteToJsConsoleAndLogFile("Archive could not be created!");

		callbackResultArgs[0] = String::New(ea->error.c_str());

		if (ea->errorCallback->IsCallable()){
			ea->errorCallback->Call(Context::GetCurrent()->Global(), 1, callbackResultArgs);
		}
	}

	return;
}

/* ********************************************************************************************************************************** */
/* PUBLIC  MEMBERS */
/* ********************************************************************************************************************************** */
void Greenutils::Init(Handle<Object> target) {
	// Prepare constructor template
	Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
	tpl->SetClassName(String::NewSymbol("Greenutils"));
	tpl->InstanceTemplate()->SetInternalFieldCount(1);

	// Prototype
	// *************************************************************************************************************************
	/* JS Console Related Exports */
	tpl->PrototypeTemplate()->Set(String::NewSymbol("enableConsole"), FunctionTemplate::New(enableConsole)->GetFunction());
	tpl->PrototypeTemplate()->Set(String::NewSymbol("disableConsole"), FunctionTemplate::New(disableConsole)->GetFunction());

	/* Zip / Unzip Exports */
	tpl->PrototypeTemplate()->Set(String::NewSymbol("extractArchive"), FunctionTemplate::New(extractArchive)->GetFunction());
	tpl->PrototypeTemplate()->Set(String::NewSymbol("createArchive"), FunctionTemplate::New(createArchive)->GetFunction());

	/* Common (Global) Exports */
	tpl->PrototypeTemplate()->Set(String::NewSymbol("move"), FunctionTemplate::New(move)->GetFunction());
	tpl->PrototypeTemplate()->Set(String::NewSymbol("sleep"), FunctionTemplate::New(sleep)->GetFunction());
	tpl->PrototypeTemplate()->Set(String::NewSymbol("getOS"), FunctionTemplate::New(getOS)->GetFunction());
	tpl->PrototypeTemplate()->Set(String::NewSymbol("enableWriteToLog"), FunctionTemplate::New(enableWriteToLog)->GetFunction());
	tpl->PrototypeTemplate()->Set(String::NewSymbol("disableWriteToLog"), FunctionTemplate::New(disableWriteToLog)->GetFunction());

	// *************************************************************************************************************************

	constructor = Persistent<Function>::New(tpl->GetFunction());
	// target->Set(String::NewSymbol("Greenutils"), constructor);
}

Handle<Value> Greenutils::NewInstance() {
	HandleScope scope;
	Local<Object> instance = constructor->NewInstance();
	return scope.Close(instance);
}

Handle<Value> Greenutils::createArchive(const Arguments& args){
	HandleScope scope;

	CUtils::WriteToJsConsoleAndLogFile("Create Archive (Zip)");
	CUtils::WriteToJsConsoleAndLogFile("***************************************************************************");

	// unzip(const char *zipfilename, const char *dirname, const char *password) {

	String::Utf8Value param1(args[0]->ToString());
	String::Utf8Value param2(args[1]->ToString());
	String::Utf8Value param3(args[2]->ToString());
	String::Utf8Value param4(args[3]->ToString());

	string sZipFileName = string(*param1);
	string sSourceDir = string(*param2);
	string sPassword = string(*param3);
	int iCompressionLevel = CUtils::ToInt(string(*param4).c_str());

	// And cast/transform them to our destination types
	sZipFileName = CUtils::Escape(sZipFileName);
	sSourceDir = CUtils::Escape(sSourceDir);
	sPassword = CUtils::Escape(sPassword);


	// Prepare Extract Archive object
	CreateExtractArchive *ea = new CreateExtractArchive;
	ea->successCallback = Persistent<Function>::New(Local<Function>::Cast(args[4]));
	ea->errorCallback = Persistent<Function>::New(Local<Function>::Cast(args[5]));
	ea->targetFile = sZipFileName;
	ea->targetFolder = sSourceDir;
	ea->password = sPassword;
	ea->compressionLevel = iCompressionLevel;
	ea->bSuccess = false;


	// End worker if source dir doesn't exist
	if (!CUtils::DirectoryExists(ea->targetFolder.c_str())){
		ea->bSuccess = false;
		ea->error = "Could not find folder to process: " + ea->targetFolder;

		// Calling error callback here
		Handle<Value> callbackResultArgs[1];
		callbackResultArgs[0] = String::New(ea->error.c_str());
		ea->errorCallback->Call(Context::GetCurrent()->Global(), 1, callbackResultArgs);

		CUtils::WriteToJsConsoleAndLogFile(ea->error);
		CUtils::WriteToJsConsoleAndLogFile("***************************************************************************");

		return scope.Close(Undefined());
	}

	CUtils::WriteToJsConsoleAndLogFile("The contents of the folder " + sSourceDir + " will be archived into " + CUtils::GetFilename(sZipFileName));

	if (sPassword.length() <= 0){
		CUtils::WriteToJsConsoleAndLogFile("Not using a password.");
	}
	else {
		CUtils::WriteToJsConsoleAndLogFile("Using a password.");
	}

	// Prepare background worker thread
	uv_work_t *req = new uv_work_t;
	req->data = ea;

	// Call background worker thread (The Async Way - Note: Do Not Call WriteToJsConsole when in a background worker thread!)
	uv_queue_work(uv_default_loop(), req, createArchiveWorker, (uv_after_work_cb)createArchiveWorkerComplete);

	// The Sync Way
	// -->createArchiveWorker(req);
	// --> createArchiveWorkerComplete(req);

	return scope.Close(Undefined());

}

Handle<Value> Greenutils::extractArchive(const Arguments& args){
	HandleScope scope;

	CUtils::WriteToJsConsoleAndLogFile("Extract Archive (Unzip)");
	CUtils::WriteToJsConsoleAndLogFile("***************************************************************************");

	// unzip(const char *zipfilename, const char *dirname, const char *password) {

	String::Utf8Value param1(args[0]->ToString());
	String::Utf8Value param2(args[1]->ToString());
	String::Utf8Value param3(args[2]->ToString());

	string sZipFileName = string(*param1);
	string sTargetDir = string(*param2);
	string sPassword = string(*param3);

	// And cast/transform them to our destination types
	sZipFileName = CUtils::Escape(sZipFileName);
	sTargetDir = CUtils::Escape(sTargetDir);
	sPassword = CUtils::Escape(sPassword);

	// Prepare Extract Archive object
	CreateExtractArchive *ea = new CreateExtractArchive;
	ea->successCallback = Persistent<Function>::New(Local<Function>::Cast(args[3]));
	ea->errorCallback = Persistent<Function>::New(Local<Function>::Cast(args[4]));
	ea->targetFile = sZipFileName;
	ea->targetFolder = sTargetDir;
	ea->password = sPassword;
	ea->bSuccess = false;

	// End worker if file doesn't exist
	if (!CUtils::FileExists(ea->targetFile.c_str())){
		ea->bSuccess = false;
		ea->error = "Could not find target file " + ea->targetFile;

		// Calling error callback here
		Handle<Value> callbackResultArgs[1];
		callbackResultArgs[0] = String::New(ea->error.c_str());
		ea->errorCallback->Call(Context::GetCurrent()->Global(), 1, callbackResultArgs);

		CUtils::WriteToJsConsoleAndLogFile(ea->error);
		CUtils::WriteToJsConsoleAndLogFile("***************************************************************************");

		return scope.Close(Undefined());
	}

	// End worker if target dir doesn't exist
	if (!CUtils::DirectoryExists(ea->targetFolder.c_str())){
		ea->bSuccess = false;
		ea->error = "Could not find target folder " + ea->targetFolder;

		// Calling error callback here
		Handle<Value> callbackResultArgs[1];
		callbackResultArgs[0] = String::New(ea->error.c_str());
		ea->errorCallback->Call(Context::GetCurrent()->Global(), 1, callbackResultArgs);

		CUtils::WriteToJsConsoleAndLogFile(ea->error);
		CUtils::WriteToJsConsoleAndLogFile("***************************************************************************");

		return scope.Close(Undefined());
	}

	CUtils::WriteToJsConsoleAndLogFile("The archive file " + CUtils::GetFilename(sZipFileName) + " will be extracted to");
	CUtils::WriteToJsConsoleAndLogFile(sTargetDir);

	if (sPassword.length() <= 0){
		CUtils::WriteToJsConsoleAndLogFile("Not using a password.");
	}
	else {
		CUtils::WriteToJsConsoleAndLogFile("Using a password.");
	}

	// Prepare background worker thread
	uv_work_t *req = new uv_work_t;
	req->data = ea;

	// Call background worker thread (The Async Way - Note: Do Not Call WriteToJsConsole when in a background worker thread!)
	uv_queue_work(uv_default_loop(), req, extractArchiveWorker, (uv_after_work_cb)extractArchiveWorkerComplete);
	
	// The Sync Way
	// --> extractArchiveWorker(req);
	// --> extractArchiveWorkerComplete(req);

	return scope.Close(Undefined());
}

Handle<Value> Greenutils::enableConsole(const Arguments& args){
	HandleScope scope;
	CUtils::WriteToJsEnabled = true;
	return scope.Close(Undefined());
}

Handle<Value> Greenutils::disableConsole(const Arguments& args){
	HandleScope scope;
	CUtils::WriteToJsEnabled = false;
	return scope.Close(Undefined());
}

Handle<Value> Greenutils::enableWriteToLog(const Arguments& args){
	HandleScope scope;

	String::Utf8Value param1(args[0]->ToString());
	CUtils::WriteToLogFileLocation = string(*param1);
	CUtils::WriteToLogEnabled = true;

	return scope.Close(Undefined());
}

Handle<Value> Greenutils::disableWriteToLog(const Arguments& args){
	HandleScope scope;

	CUtils::WriteToLogEnabled = false;
	return scope.Close(Undefined());
}

Handle<Value> Greenutils::sleep(const Arguments& args){
	HandleScope scope;

	String::Utf8Value param1(args[0]->ToString());
	string sVal = string(*param1);
	int iVal = CUtils::ToInt(sVal.c_str());

	CUtils::Sleep(iVal);
	return scope.Close(Undefined());
}

Handle<Value> Greenutils::move(const Arguments& args){
	HandleScope scope;

	String::Utf8Value param1(args[0]->ToString());
	String::Utf8Value param2(args[1]->ToString());

	string sSource = string(*param1);
	string sTarget = string(*param2);
	rename(sSource.c_str(), sTarget.c_str());
	return scope.Close(Undefined());
}

Handle<Value> Greenutils::getOS(const Arguments& args){
	HandleScope scope;

#ifdef _WIN32
	 string os = "win";
#endif
#ifdef __APPLE__
	 string os = "apple";
#endif
#ifdef __linux__
	 string os = "linux";
#endif

	return scope.Close(String::New(os.c_str()));
}

