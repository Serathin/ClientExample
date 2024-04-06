#include <QCoreApplication>
#include <QDataStream>
#include <QCryptographicHash>
#include <QTextStream>
#include <QTcpSocket>

namespace {

constexpr char kRegistration[] = "registration";
constexpr char kAuthorization[] = "authorization";

} // namespace

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QTcpSocket socket;
    QObject::connect(&socket, &QTcpSocket::connected, [&socket] {
        QString method, login, password;
        QTextStream in(stdin, QIODevice::ReadOnly), out(stdout, QIODevice::WriteOnly);
        QDataStream stream(&socket);
        out << QString("Input method (%1/%2): ").arg(kRegistration).arg(kAuthorization);
        out.flush();
        in >> method;
        while (method != kRegistration && method != kAuthorization) {
            out << "Try again.\n";
            out.flush();
            in >> method;
        }
        out << "Input login: ";
        out.flush();
        in >> login;
        out << "Input password: ";
        out.flush();
        in >> password;
        password = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Algorithm::Sha512);
        stream << method << login << password;
    });
    QObject::connect(&socket, &QTcpSocket::readyRead, [&socket] {
        QTextStream out(stdout);
        QDataStream stream(&socket);
        QString msg;
        stream >> msg;
        out << msg;
        out.flush();
        QCoreApplication::exit(0);
    });
    socket.connectToHost("127.0.0.1", 12345);

    return app.exec();
}
