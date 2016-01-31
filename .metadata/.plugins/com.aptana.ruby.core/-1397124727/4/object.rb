class Object < BasicObject
  include Kernel

  ARGF = ARGF
  ARGV = []
  Addrinfo = Addrinfo
  ArgumentError = ArgumentError
  Array = Array
  BasicObject = BasicObject
  BasicSocket = BasicSocket
  Bignum = Bignum
  Binding = Binding
  CGI = CGI
  CROSS_COMPILING = nil
  Class = Class
  Comparable = Comparable
  Complex = Complex
  ConditionVariable = Thread::ConditionVariable
  Config = RbConfig::Obsolete
  Data = Data
  Date = Date
  DateTime = DateTime
  Delegator = Delegator
  Digest = Digest
  Dir = Dir
  ENV = {"SHLVL"=>"2", "DBUS_STARTER_BUS_TYPE"=>"session", "SESSION_MANAGER"=>"local/woodstock:@/tmp/.ICE-unix/762,unix/woodstock:/tmp/.ICE-unix/762", "GNOME_DESKTOP_SESSION_ID"=>"this-is-deprecated", "DBUS_STARTER_ADDRESS"=>"unix:abstract=/tmp/dbus-LvGB94Byn4,guid=0af4cf05b8be8007cb9106c956ada878", "IM_CONFIG_PHASE"=>"1", "GDMSESSION"=>"gnome", "MANDATORY_PATH"=>"/usr/share/gconf/gnome.mandatory.path", "XDG_DATA_DIRS"=>"/usr/share/gnome:/usr/local/share/:/usr/share/", "PWD"=>"/home/lucie/T\xC3\xA9l\xC3\xA9chargements/Aptana_Studio_3", "QT_IM_MODULE"=>"ibus", "LOGNAME"=>"lucie", "_"=>"/usr/bin/dbus-update-activation-environment", "XDG_SESSION_TYPE"=>"x11", "XMODIFIERS"=>"@im=ibus", "SSH_AUTH_SOCK"=>"/run/user/1000/keyring/ssh", "XDG_MENU_PREFIX"=>"gnome-", "DBUS_SESSION_BUS_ADDRESS"=>"unix:abstract=/tmp/dbus-LvGB94Byn4,guid=0af4cf05b8be8007cb9106c956ada878", "SHELL"=>"/bin/bash", "XDG_CONFIG_DIRS"=>"/etc/xdg/xdg-gnome:/etc/xdg", "PATH"=>"/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games", "XDG_CURRENT_DESKTOP"=>"GNOME", "DESKTOP_SESSION"=>"gnome", "APTANA_VERSION"=>"3.6.2.1413590556", "DISPLAY"=>":0.0", "USER"=>"lucie", "HOME"=>"/home/lucie", "XAUTHORITY"=>"/run/user/1000/gdm/Xauthority", "XDG_SEAT"=>"seat0", "WINDOWPATH"=>"7", "XDG_SESSION_ID"=>"c1", "DEFAULTS_PATH"=>"/usr/share/gconf/gnome.default.path", "USERNAME"=>"lucie", "XDG_SESSION_DESKTOP"=>"gnome", "XDG_VTNR"=>"7", "XDG_RUNTIME_DIR"=>"/run/user/1000", "LANG"=>"fr_FR.UTF-8"}
  EOFError = EOFError
  Encoding = Encoding
  EncodingError = EncodingError
  Enumerable = Enumerable
  Enumerator = Enumerator
  Errno = Errno
  Etc = Etc
  Exception = Exception
  FALSE = false
  FalseClass = FalseClass
  Fcntl = Fcntl
  Fiber = Fiber
  FiberError = FiberError
  File = File
  FileTest = FileTest
  FileUtils = FileUtils
  Fixnum = Fixnum
  Float = Float
  FloatDomainError = FloatDomainError
  GC = GC
  Gem = Gem
  Hash = Hash
  IO = IO
  IOError = IOError
  IPSocket = IPSocket
  IndexError = IndexError
  Integer = Integer
  Interrupt = Interrupt
  Kernel = Kernel
  KeyError = KeyError
  LoadError = LoadError
  LocalJumpError = LocalJumpError
  Marshal = Marshal
  MatchData = MatchData
  Math = Math
  Method = Method
  Module = Module
  Monitor = Monitor
  MonitorMixin = MonitorMixin
  Mutex = Mutex
  NIL = nil
  NameError = NameError
  Net = Net
  NilClass = NilClass
  NoMemoryError = NoMemoryError
  NoMethodError = NoMethodError
  NotImplementedError = NotImplementedError
  Numeric = Numeric
  OUTPUT_PATH = "/home/lucie/workspace/Bioinformatics-Ruby/.metadata/.plugins/com.aptana.ruby.core/-1397124727/4/"
  Object = Object
  ObjectSpace = ObjectSpace
  OpenSSL = OpenSSL
  OptionParser = OptionParser
  Proc = Proc
  Process = Process
  Queue = Thread::Queue
  RUBY_COPYRIGHT = "ruby - Copyright (C) 1993-2014 Yukihiro Matsumoto"
  RUBY_DESCRIPTION = "ruby 2.1.5p273 (2014-11-13) [x86_64-linux-gnu]"
  RUBY_ENGINE = "ruby"
  RUBY_PATCHLEVEL = 273
  RUBY_PLATFORM = "x86_64-linux-gnu"
  RUBY_RELEASE_DATE = "2014-11-13"
  RUBY_REVISION = 0
  RUBY_VERSION = "2.1.5"
  Random = Random
  Range = Range
  RangeError = RangeError
  Rational = Rational
  RbConfig = RbConfig
  Regexp = Regexp
  RegexpError = RegexpError
  Resolv = Resolv
  RubyVM = RubyVM
  RuntimeError = RuntimeError
  STDERR = IO.new
  STDIN = IO.new
  STDOUT = IO.new
  ScanError = StringScanner::Error
  ScriptError = ScriptError
  SecureRandom = SecureRandom
  SecurityError = SecurityError
  Signal = Signal
  SignalException = SignalException
  SimpleDelegator = SimpleDelegator
  SizedQueue = Thread::SizedQueue
  Socket = Socket
  SocketError = SocketError
  StandardError = StandardError
  StopIteration = StopIteration
  String = String
  StringIO = StringIO
  StringScanner = StringScanner
  Struct = Struct
  Symbol = Symbol
  SyntaxError = SyntaxError
  SystemCallError = SystemCallError
  SystemExit = SystemExit
  SystemStackError = SystemStackError
  TCPServer = TCPServer
  TCPSocket = TCPSocket
  TOPLEVEL_BINDING = #<Binding:0x00000000f02558>
  TRUE = true
  TSort = TSort
  Tempfile = Tempfile
  Thread = Thread
  ThreadError = ThreadError
  ThreadGroup = ThreadGroup
  Time = Time
  Timeout = Timeout
  TimeoutError = Timeout::Error
  TracePoint = TracePoint
  TrueClass = TrueClass
  TypeError = TypeError
  UDPSocket = UDPSocket
  UNIXServer = UNIXServer
  UNIXSocket = UNIXSocket
  URI = URI
  UnboundMethod = UnboundMethod
  ZeroDivisionError = ZeroDivisionError
  Zlib = Zlib
  fatal = nil



  protected


  private

  def DelegateClass(arg0)
  end

  def Digest(arg0)
  end

  def dir_names(arg0)
  end

  def file_name(arg0)
  end

  def get_classes
  end

  def grab_instance_method(arg0, arg1)
  end

  def print_args(arg0)
  end

  def print_instance_method(arg0, arg1)
  end

  def print_method(arg0, arg1, arg2, arg3, arg4, *rest)
  end

  def print_type(arg0)
  end

  def print_value(arg0)
  end

  def timeout(arg0, arg1, arg2, *rest)
  end

end
