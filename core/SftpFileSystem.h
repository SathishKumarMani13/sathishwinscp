//---------------------------------------------------------------------------
#ifndef SftpFileSystemH
#define SftpFileSystemH

#include <FileSystems.h>
//---------------------------------------------------------------------------
class TSFTPPacket;
//---------------------------------------------------------------------------
class TSFTPFileSystem : public TCustomFileSystem
{
friend class TSFTPPacket;
public:
  __fastcall TSFTPFileSystem(TTerminal * ATerminal);
  virtual __fastcall ~TSFTPFileSystem();

  virtual void __fastcall AnyCommand(const AnsiString Command);
  virtual void __fastcall ChangeDirectory(const AnsiString Directory);
/*  virtual void __fastcall ChangeGroup(const AnsiString FileName,
    const AnsiString Group, bool Recursive);
  virtual void __fastcall ChangeMode(const AnsiString FileName,
    const TRights * Rights, bool Recursive);
  virtual void __fastcall ChangeOwner(const AnsiString FileName,
    const AnsiString Owner, bool Recursive);*/
  virtual void __fastcall ChangeFileProperties(const AnsiString FileName,
    const TRemoteFile * File, const TRemoteProperties * Properties);
  virtual void __fastcall CopyToLocal(TStrings * FilesToCopy,
    const AnsiString TargetDir, const TCopyParamType * CopyParam,
    int Params, TFileOperationProgressType * OperationProgress,
    bool & DisconnectWhenComplete);
  virtual void __fastcall CopyToRemote(TStrings * FilesToCopy,
    const AnsiString TargetDir, const TCopyParamType * CopyParam,
    int Params, TFileOperationProgressType * OperationProgress,
    bool & DisconnectWhenComplete);
  virtual void __fastcall CreateDirectory(const AnsiString DirName,
    const TRemoteProperties * Properties);
  virtual void __fastcall CreateLink(const AnsiString FileName, const AnsiString PointTo, bool Symbolic);
  virtual void __fastcall DeleteFile(const AnsiString FileName,
    const TRemoteFile * File = NULL, bool Recursive = false);
  virtual void __fastcall DoStartup();
  virtual void __fastcall HomeDirectory();
  virtual bool __fastcall IsCapable(int Capability) const;
  virtual void __fastcall LookupUserGroups();
  virtual void __fastcall ReadCurrentDirectory();
  virtual void __fastcall ReadDirectory(TRemoteFileList * FileList);
  virtual void __fastcall ReadFile(const AnsiString FileName,
    TRemoteFile *& File);
  virtual void __fastcall ReadSymlink(TRemoteFile * SymlinkFile,
    TRemoteFile *& File);
  virtual void __fastcall RenameFile(const AnsiString FileName,
    const AnsiString NewName);

protected:
  int FVersion;
  AnsiString FCurrentDirectory;
  AnsiString FDirectoryToChangeTo;
  AnsiString FHomeDirectory;
  TList * FPacketReservations;
  Variant FPacketNumbers;
  char FPreviousLoggedPacket;
  int FNotLoggedPackets;

/*  void __fastcall DoChangeProperties(const AnsiString FileName,
    const TRemoteFile * File, void * Properties);*/
  void __fastcall CustomReadFile(const AnsiString FileName,
    TRemoteFile *& File, char Type, TRemoteFile * ALinkedByFile = NULL);
  /*void __fastcall DoDeleteFile(const AnsiString FileName,
    const TRemoteFile * File, void * DummyParam);*/
  virtual AnsiString __fastcall GetCurrentDirectory();
  AnsiString __fastcall GetHomeDirectory();
  unsigned long __fastcall GotStatusPacket(TSFTPPacket * Packet, int AllowStatus);
  bool __fastcall inline IsAbsolutePath(const AnsiString Path);
  bool __fastcall inline RemoteFileExists(const AnsiString FullPath, TRemoteFile ** File = NULL);
  TRemoteFile * __fastcall LoadFile(TSFTPPacket * Packet,
    TRemoteFile * ALinkedByFile);
  AnsiString __fastcall inline LocalCanonify(const AnsiString Path);
  AnsiString __fastcall Canonify(AnsiString Path);
  AnsiString __fastcall RealPath(const AnsiString Path);
  AnsiString __fastcall RealPath(const AnsiString Path, const AnsiString BaseDir);
  void __fastcall ReserveResponse(const TSFTPPacket * Packet,
    TSFTPPacket * Response);
  virtual void __fastcall SetCurrentDirectory(AnsiString value);
  virtual AnsiString __fastcall GetProtocolName() const;
  void __fastcall ReceivePacket(TSFTPPacket * Packet, int ExpectedType = -1);
  void __fastcall RemoveReservation(int Reservation);
  void __fastcall SendPacket(const TSFTPPacket * Packet);
  void __fastcall ReceiveResponse(
    const TSFTPPacket * Packet, TSFTPPacket * Response, int ExpectedType = -1);
  void __fastcall SendPacketAndReceiveResponse(
    const TSFTPPacket * Packet, TSFTPPacket * Response, int ExpectedType = -1);
  void __fastcall UnreserveResponse(TSFTPPacket * Response);
  void __fastcall TryOpenDirectory(const AnsiString Directory);

  void __fastcall SFTPSource(const AnsiString FileName,
    const AnsiString TargetDir, const TCopyParamType * CopyParam, int Params,
    TFileOperationProgressType * OperationProgress);
  int __fastcall SFTPOpenRemote(void * AOpenParams, void * /*Param2*/);
  void __fastcall SFTPDirectorySource(const AnsiString DirectoryName,
    const AnsiString TargetDir, int /*Attrs*/, const TCopyParamType * CopyParam,
    int Params, TFileOperationProgressType * OperationProgress);
  void __fastcall SFTPConfirmOverwrite(const AnsiString FileName,
    TFileOperationProgressType * OperationProgress);
  bool SFTPConfirmResume(const AnsiString DestFileName, bool PartialBiggerThanSource,
    TFileOperationProgressType * OperationProgress);
  void __fastcall SFTPSink(const AnsiString FileName,
    const TRemoteFile * File, const AnsiString TargetDir,
    const TCopyParamType * CopyParam, int Params,
    TFileOperationProgressType * OperationProgress);
  void __fastcall SFTPSinkFile(AnsiString FileName,
    const TRemoteFile * File, void * Param);
};
//---------------------------------------------------------------------------
#endif // SftpFileSystemH
