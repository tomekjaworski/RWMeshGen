#if !defined (_NATIVE_EXCEPTIONS_HPP_)
#define _NATIVE_EXCEPTIONS_HPP_

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the EXCEPTIONS_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// EXCEPTIONS_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef NATIVEEXCEPTIONS_EXPORTS
#define NATIVEEXCEPTIONS_API __declspec(dllexport)
#else
#define NATIVEEXCEPTIONS_API __declspec(dllimport)
#endif

// ############################################################
// ### A base class for exceptions
// ############################################################
#include <tchar.h>
#include <string>

typedef std::basic_string<TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR>> tstring;

namespace NativeExceptions
{

	class NATIVEEXCEPTIONS_API Exception {

	private:
		tstring exception_class_name;

		void InitException(void);
		void SetInnerException(const Exception & e);

	protected:
		tstring ToStringBuffer;
		void SetExceptionClassName(const TCHAR *);

	public:
		tstring Message;
		Exception * InnerException;

		Exception(void);
		Exception(const TCHAR * message);
		Exception(const Exception & inner_exception);
		Exception(const TCHAR * message, Exception & inner_exception);

		const TCHAR * GetExceptionClassName(void);

		virtual ~Exception(void);
		virtual const TCHAR * ToString(void);
		//virtual CException * Clone(void);

	};

	// ############################################################
	// ### Geometry processing exception
	// ############################################################

	class NATIVEEXCEPTIONS_API GeometryException :
		public Exception
	{
	public:
		GeometryException(const TCHAR * message);
		virtual ~GeometryException(void);
		//virtual CGeometryException * Clone(void);
	};

	// ############################################################
	// ### File input/output exception
	// ############################################################

	class NATIVEEXCEPTIONS_API IOException :
		public Exception
	{
	public:
		tstring FileName;
		int ErrorCode;

		IOException(const TCHAR * file_name, int error_code, const TCHAR * message = _T(""));
		virtual ~IOException(void);
		virtual const TCHAR * ToString(void);
		//virtual CGeometryException * Clone(void);
	};

	// ############################################################
	// ### Memory exception
	// ############################################################

	class NATIVEEXCEPTIONS_API OutOfMemoryException :
		public Exception
	{
	public:
		size_t RequestedMemory;

		 OutOfMemoryException(const TCHAR * message, size_t requested_memory = 0);
		virtual ~OutOfMemoryException(void);
		virtual const TCHAR * ToString(void);
	};

	// ############################################################
	// ### Out of range exception
	// ############################################################

	class NATIVEEXCEPTIONS_API RangeException :
		public Exception
	{
	public:
		double RangeValue;

		RangeException(const TCHAR * message, double value = 0.0);
		virtual ~RangeException(void);
		virtual const TCHAR * ToString(void);
	};


	// ############################################################
	// ### Mesh processing exception
	// ############################################################

	class NATIVEEXCEPTIONS_API MeshException :
		public Exception
	{
	public:
		MeshException(const TCHAR * message);
		virtual ~MeshException(void);
	};

	// ############################################################

	class NATIVEEXCEPTIONS_API NotImplementedException :
		public Exception
	{
	public:
		NotImplementedException(const TCHAR * message);
		virtual ~NotImplementedException(void);
	};


	class NATIVEEXCEPTIONS_API ModelException :
		public Exception
	{
	public:
		ModelException(const TCHAR * message);
		virtual ~ModelException(void);
	};
};

#endif // _NATIVE_EXCEPTIONS_HPP_