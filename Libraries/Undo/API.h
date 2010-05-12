#pragma once

#ifdef UNDO_EXPORTS
#define UNDO_API __declspec(dllexport)
#else
#define UNDO_API __declspec(dllimport)
#endif