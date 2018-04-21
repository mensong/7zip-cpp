#pragma once


#include <vector>
#include <atlbase.h>
#include "SevenZipLibrary.h"
#include "SevenZipArchive.h"
#include "FileInfo.h"
#include "CompressionFormat.h"
#include "CompressionLevel.h"
#include "ProgressCallback.h"


namespace SevenZip
{
	class SevenZipCompressor : public SevenZipArchive
	{
	public:

		SevenZipCompressor( const SevenZipLibrary& library, const TString& archivePath);
		virtual ~SevenZipCompressor();

		// Includes the directory as the root in the archive, e.g. specifying "C:\Temp\MyFolder"
		// makes "MyFolder" the single root item in archive with the files within it included.
		virtual bool AddDirectory(const TString& directory, bool includeSubdirs = true);

		// Excludes the last directory as the root in the archive, its contents are at root instead. E.g.
		// specifying "C:\Temp\MyFolder" make the files in "MyFolder" the root items in the archive.
		virtual bool AddFiles( const TString& directory, const TString& searchFilter, bool includeSubdirs = true);
		virtual bool AddAllFiles( const TString& directory, bool includeSubdirs = true);

		// Compress just this single file as the root item in the archive.
		virtual bool AddFile(const TString& filePath);

		// Compress list of files
		virtual bool DoCompress(ProgressCallback* callback = nullptr);

		void ClearList() { m_fileList.clear(); }
		void UseAbsolutePaths(bool absolute) { m_absolutePath = absolute; }
		bool CheckValidFormat();

	private:
		std::vector< intl::FilePathInfo > m_fileList; // list of files to compress
		bool m_absolutePath;
		CComPtr< IStream > OpenArchiveStream();
		bool AddFilesToList(const TString& directory, const TString& searchPattern, const TString& pathPrefix, bool recursion);
		bool SetCompressionProperties( IUnknown* outArchive );
	};
}
