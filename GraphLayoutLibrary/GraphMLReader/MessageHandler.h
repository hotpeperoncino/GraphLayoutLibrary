#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include <QAbstractMessageHandler>
#include <GraphLayoutLibrary_global.h>

/**
 * @brief The MessageHandler class
 *
 * The class provides helpers to handle messages. Inherited from QAbstractMessageHandler.
 */
class GRAPHLAYOUTLIBRARYSHARED_EXPORT MessageHandler : public QAbstractMessageHandler
{
    /* Adding subclass to QAbstractMessageHandler needs .pro file to be configured.
     * We need to add QT += xmlpatterns
     */
public:
	/** @name Creators
     *  The methods under this section are responsible for constructing
     *  an instance of type MessageHandler.
	 *  Uses default constructor of QAbstractMessageHandler.
     */

    //@{
	/**
     Constructs new object of type MessageHandler.
    @pre none

    @param none

    @return none

    @throw none
    */
    MessageHandler() : QAbstractMessageHandler(0)
    {
    }
	//@}


 
 /** @name Queries
 *  The methods under this section are responsible for accessing
 *  an instance of type MessageHandler.
 */
//@{
	/**
        This method returns the description from the messeage already written

        @pre none

        @param none

        @return message in string

        @throw none
    */
    QString getStatusMessage() const
    {
        return m_sDescription;
    }

    /**
        This method returns line number for the message.

        @pre none

        @param none

        @return line number

        @throw none
    */
    int getLineNumber() const
    {
        return m_sourceLocation.line();
    }

    /**
        This method returns colun number for the message.

        @pre none

        @param none

        @return column number

        @throw none
    */
    int getColumnNumber() const
    {
        return m_sourceLocation.column();
    }


    /**
        This method returns type of the message

        @pre none

        @param none

        @return message type in string

        @throw none
    */
    QtMsgType getMessageType() const
    {
        return m_messageType;
    }


    /**
        This method returns message id.

        @pre none

        @param none

        @return message id

        @throw none
    */
    QUrl getMessageId() const
    {
        return m_messageId;
    }
//@}
    

protected:
/** @name Modifiers
  *  The methods under this section are responsible for modifying
  *  an instance of MessageHandler.
  */
 //@{
	/**
        This method handles the message and set its parameters.

        @pre none

        @param type
            type of message

        @param description
            message description

        @param identifier
            url identifier

        @param sourceLocation
            location for the source of message

        @return none

        @throw none
    */
    virtual void handleMessage(QtMsgType type, const QString &description,
                               const QUrl &identifier, const QSourceLocation &sourceLocation)
    {
        Q_UNUSED(type);
        Q_UNUSED(identifier);

        m_messageType = type;
        m_sDescription = description;
        m_messageId = identifier;
        m_sourceLocation = sourceLocation;
    }
 //@}
    

private:
    QtMsgType m_messageType; /*!< QtMsgType to store type of message to be handled. */
    QString m_sDescription; /*!< QString to store description of message. */
    QUrl m_messageId; /*!< QUrl to store message ID. */
    QSourceLocation m_sourceLocation; /*!< QSourceLocation to store location for the source of message. */
};

#endif // MESSAGEHANDLER_H
