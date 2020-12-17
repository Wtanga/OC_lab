#pragma once

/*Поскольку LIBRARY_EXPORTS не будет определен в проекте, использующем DLL, 
когда этот проект включает заголовочный файл вашей библиотеки, все функции будут
импортированы вместо него.*/

#ifdef BIGBROTHERLIBRARY_EXPORTS
#define BIGBROTHERLIBRARY_API __declspec(dllexport)
#else
#define BIGBROTHERLIBRARY_API __declspec(dllimport)
#endif

extern "C" BIGBROTHERLIBRARY_API void SetFarHook(void);
extern "C" BIGBROTHERLIBRARY_API void UninstallFarHook(void);
