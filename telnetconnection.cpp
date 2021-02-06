#include "telnetconnection.h"
#include <QDebug>
#include <QLoggingCategory>
#include <QEventLoop>


Q_LOGGING_CATEGORY(log_telnetconnection, "Telnet")
Q_LOGGING_CATEGORY(log_lowtelnetconnection, "LowLevelTelnet")

#define sDebug() QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC, log_telnetconnection().categoryName()).debug() << debugName
#define lDebug() QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC, log_lowtelnetconnection().categoryName()).debug() << debugName

// TELNET STUFF


#define CLOVER_SHELL_PROMPT "root@madmonkey:~# "
#define CHANGED_PROMPT      "KLDFJLD45SDKL@4"
#define CLOVER_SHELL_PROMPT_SIZE 15
#define FUCK_LINE_SIZE 80
#define DO 0xfd
#define WONT 0xfc
#define WILL 0xfb
#define DONT 0xfe
#define CMD 0xff
#define CMD_ECHO 1
#define NAWS 31
#define ITPC 244
#define SGA 3



TelnetConnection::TelnetConnection(const QString &hostname, int port, const QString &user, const QString &password) : QObject()
{
    qRegisterMetaType<TelnetConnection::ConnectionError>("TelnetConnection::ConnectionError");
    m_host = hostname;
    m_port = port;
    m_user = user;
    m_password = password;
    m_istate = Init;
    connect(&socket, SIGNAL(readyRead()), this, SLOT(onSocketReadReady()));
    connect(&socket, SIGNAL(connected()), this, SLOT(onSocketConnected()));
    connect(&socket, SIGNAL(disconnected()), this, SLOT(onSocketDisconnected()));
    connect(&socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onSocketError(QAbstractSocket::SocketError)));
    m_state = Offline;
    debugName = "default";
    oneCommandMode = false;
    nbRM = 0;
    sDebug() << "Creating telnetconnection object";
    charToCheck = FUCK_LINE_SIZE - CLOVER_SHELL_PROMPT_SIZE;
    telnetCmd[WONT] = "WONT";
    telnetCmd[WILL] = "WILL";
    telnetCmd[DO] = "DO";
    telnetCmd[DONT] = "DONT";
    telnetCmd[255] = "CMD";
    telnetCmd[CMD_ECHO] = "ECHO";
    telnetCmd[NAWS] = "NAWS";
    telnetCmd[SGA] = "SGA";
}

QByteArray TelnetConnection::syncExecuteCommand(QString cmd)
{
    if (!(m_state == Ready || m_state == Connected))
        return QByteArray();
    QEventLoop loop;
    executeCommand(cmd);
    connect(this, SIGNAL(commandReturn(QByteArray)), &loop, SLOT(quit()));
    loop.exec();
    sDebug() << "syncCommand done";
    return lastCommandReturn;
}

TelnetConnection::State TelnetConnection::state()
{
    return m_state;
}

void TelnetConnection::setOneCommandMode(bool mode)
{
    oneCommandMode = mode;
}

void TelnetConnection::conneect()
{
    sDebug() << "Connection requested" << m_istate;
    if (m_istate != Init)
        return;
    socket.connectToHost(m_host, m_port);
    nbRM = 0;
    charToCheck = FUCK_LINE_SIZE - CLOVER_SHELL_PROMPT_SIZE;
    m_istate = AttemptConnection;
}

void TelnetConnection::executeCommand(QString toSend)
{
    sDebug() << "Executing : " << toSend;
    writeToTelnet(toSend.toLatin1() + "\r\n");
}

void TelnetConnection::close()
{
    if (m_state == Offline)
        return;
    sDebug() << "Closing Telnet connection";
    if (m_state == Ready)
        executeCommand("exit");
    else {
        sDebug() << "Sending interupt";
        char buf[3];
        buf[0] = '\r';
        buf[1] = '\n';
        buf[2] = 0x03;
        socket.write(buf, 3);
        socket.write("exit\r\n", 6);
    }
    if (m_state != Offline)
        socket.close();
    sDebug() << "Should be closed";
}

static void append_cmd(QByteArray& data, quint8 what, quint8 cmd)
{
    data.append(CMD);
    data.append(what);
    data.append(cmd);
}

void TelnetConnection::onSocketConnected()
{
    QByteArray cmd;
    append_cmd(cmd, DO, CMD_ECHO);
    append_cmd(cmd, WILL, SGA);
    append_cmd(cmd, DO, SGA);
    append_cmd(cmd, WONT, NAWS);
    socket.write(cmd);

    m_istate = SocketConnected;
}

void TelnetConnection::onSocketError(QAbstractSocket::SocketError error)
{
    sDebug() << socket.errorString();
    if (error == QAbstractSocket::ConnectionRefusedError)
        emit connectionError(TelnetConnection::ConnectionRefused);
    m_istate = Init;
}

void TelnetConnection::onSocketDisconnected()
{
    setState(Offline);
    m_istate = Init;
    sDebug() << "Disconnected";
    emit disconnected();
}


void TelnetConnection::onSocketReadReady()
{
  QByteArray data = socket.read(1024);
  lDebug() << "Received data on socket";
  lDebug() << "DATA:" << data;
  lDebug() << m_state << m_istate;
  if (m_istate == SocketConnected)
  {
      if (data.indexOf("Error: NES Mini is offline") != -1)
      {
          emit connectionError(TelnetConnection::SNESNotRunning);
          return;
      }
  }
  if (data.at(0) == (char) 0xFF && m_istate < IReady)
  {
      sDebug() << "Telnet cmd receive, we don't care for now";
      lDebug() << "Received : " << data;
      unsigned int cpt;
      QString   cmd;
      for (cpt = 0; cpt < data.size(); cpt++)
      {
          if (telnetCmd.contains((quint8)data.at(cpt)))
              cmd.append(telnetCmd[(quint8)data.at(cpt)] + " ");
          else
              cmd.append(QString::number((quint8)data.at(cpt), 16) + " ");
      }
      sDebug() << cmd;
      char buf[3];
      buf[0] = CMD;
      buf[1] = WILL;
      buf[2] = 34; // Line mode;
      socket.write(buf,  3);
      //return ;
  }
  if (data.indexOf(0xFF) != -1)
  {
      int pos = data.indexOf(0xFF);
      lDebug() << "Removing telnet cmd : " << data.at(pos +1);
      if (data.at(pos + 1) == (char) 0xFF)
          data.remove(pos, 3);
      else
          data.remove(pos, 2);
  }
  if (m_istate == PromptChangeWritten)
  {
      readBuffer.append(data);
      if (readBuffer == "PS1='" + QByteArray(CHANGED_PROMPT) + "'\r\n" + QByteArray(CHANGED_PROMPT))
      {
          m_istate = PromptChanged;
          readBuffer.clear();
          setState(Connected);
          //writeToTelnet("echo 'Hello world'\r\n");
          emit connected();
      }
      return ;
  }
  //qDebug() << "RAW:" << data;
  if (m_istate == LoginWritten && data.indexOf(CLOVER_SHELL_PROMPT) != -1)
  {
      m_istate = Logged;
      writeToTelnet("PS1='" + QByteArray(CHANGED_PROMPT) + "'\r\n");
      m_istate = PromptChangeWritten;
      return ;
  }
  if (data.indexOf("madmonkey login: ") != -1)
  {
      writeToTelnet("root\r\n");
      m_istate = LoginWritten;
      return;
  }
  if (m_istate < PromptChanged)
      return ;
  readBuffer.append(data);
  lDebug() << m_istate << readBuffer.size();
  // We entered the login
  // A command string has be written, we want to remove the feedback of it
  if (m_istate == DataWritten)
  {
      if (readBuffer.size() >= calcSizeSent)
      {
          m_istate = WaitingForCmd;
          readBuffer.remove(0, calcSizeSent);
          //charToCheck = FUCK_LINE_SIZE - CLOVER_SHELL_PROMPT_SIZE;
      }
  }
  //lDebug()() << "PIKOOOOOOOOOOO" << WaitingForCmd << m_istate;
  // We are waiting for the output of a command
  if (m_istate == WaitingForCmd)
  {
      lDebug() << "Waiting for command";
      int pos = readBuffer.indexOf(CHANGED_PROMPT);
      if (oneCommandMode)
      {
          lDebug() << "Command mode";
          if (readBuffer.indexOf("\r\n") != -1)
          {
              //qDebug() << "PIKOOOOOOOOOOOOOOOOOOO";
              lDebug() << "Readbuffer : " << readBuffer;
              int rPos = readBuffer.indexOf("\r\n");
              while (rPos != -1)
              {
                lDebug() << readBuffer.left(rPos);
                emit  commandReturnedNewLine(readBuffer.left(rPos));
                readBuffer.remove(0, rPos + 2);
                rPos = readBuffer.indexOf("\r\n");
              }
              readBuffer.clear();
          }
      }
      if (pos != -1)
      {
        readBuffer.remove(pos, QString(CHANGED_PROMPT).size());
        lDebug() << "======="; lDebug() << "Received shell cmd data"; lDebug() << readBuffer ; lDebug() << "=======";
        setState(Ready);
        m_istate = IReady;
        lastCommandReturn = readBuffer;
        sDebug() << "Command returned : " << lastCommandReturn;
        emit commandReturn(lastCommandReturn);
        readBuffer.clear();
        charToCheck = FUCK_LINE_SIZE - CLOVER_SHELL_PROMPT_SIZE;
      }
  }
}

void TelnetConnection::writeToTelnet(QByteArray toWrite)
{
    lDebug() << "Writing to telnet " << toWrite << m_istate;
    if (m_istate == IReady || m_istate == PromptChanged)
        m_istate = DataWritten;
    lastSent = toWrite;
    calcSizeSent = toWrite.size();
    if (calcSizeSent > FUCK_LINE_SIZE - CLOVER_SHELL_PROMPT_SIZE)
    {
        unsigned int mSize = calcSizeSent - (FUCK_LINE_SIZE - CLOVER_SHELL_PROMPT_SIZE);
        calcSizeSent += 3;
        while (mSize > FUCK_LINE_SIZE)
        {
            mSize -= FUCK_LINE_SIZE;
            calcSizeSent += 3;
        }
    }
    sDebug() << "Original size is " << toWrite.size() << "should be " << calcSizeSent;
    socket.write(toWrite);
}

void TelnetConnection::setState(TelnetConnection::State st)
{
    lDebug() << "State changed to " << st;
    m_state = st;
}
