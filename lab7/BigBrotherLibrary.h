#pragma once

/*��������� LIBRARY_EXPORTS �� ����� ��������� � �������, ������������ DLL, 
����� ���� ������ �������� ������������ ���� ����� ����������, ��� ������� �����
������������� ������ ����.*/

#ifdef BIGBROTHERLIBRARY_EXPORTS
#define BIGBROTHERLIBRARY_API __declspec(dllexport)
#else
#define BIGBROTHERLIBRARY_API __declspec(dllimport)
#endif

extern "C" BIGBROTHERLIBRARY_API void SetFarHook(void);
extern "C" BIGBROTHERLIBRARY_API void UninstallFarHook(void);
