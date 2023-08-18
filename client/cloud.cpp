#define _CRT_SECURE_NO_WARNINGS 1
#include "dataManager.hpp"
#include "util.hpp"
#include "backup.hpp"
#define BACKUP_FILE "./backup.dat"
#define BACKUP_DIR "./back_dir"
int main()
{
	//Cloud::FileUtil fu("./");
	//std::vector<std::string> array;
	//fu.ScanDirectory(&array);
	//Cloud::DataManager dataMgr(BACKUP_FILE);
	//for (auto& file : array)
	//{
	//	dataMgr.Insert(file, "TEST431ES13casTEAICSA32EIONCIAWJ123DDODafA");
	//}
	Cloud::Backup instance(BACKUP_DIR, BACKUP_FILE);
	instance.RunModule();
	return 0;
}