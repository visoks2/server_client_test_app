#pragma once

#include "Common.h"
#include "Exceptions.h"
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include "dataStorageHandler.h"
#include "socket_description.h"
#include <data.pb.h>

namespace Server
{
class MessagesHandler
{
  public:
    /**
     * @brief 	MessagesHandler c-tor
     */
    MessagesHandler()
    {
        storageHandler = std::make_shared<dataStorageHandler>();
    };
    /**
     * @brief 	MessagesHandler c-tor
     *
     * @param   aStorageHandler	    pointer to storage handler
     * @note    Used only with mocks
     */
    MessagesHandler(std::shared_ptr<dataStorageHandler> aStorageHandler)
    {
        storageHandler = aStorageHandler;
    };

    /**
     * @brief 	MessagesHandler c-tor
     */
    ~MessagesHandler() = default;

    std::string ProcessMessage(socket_description &user, std::string &aMsg)
    {
        gpb::Message parsedMsg;
        if (!parsedMsg.ParseFromString(aMsg))
        {
            throw Common::MessageParseException();
        }
        if ((!user.isAuthenticated) && (parsedMsg.action() != gpb::Message_Action::Message_Action_AUTHENTICATE))
        {
            throw Common::AuthenticationFailedException();
        }

        switch (parsedMsg.action())
        {
        case gpb::Message_Action::Message_Action_CREATE: {
            storageHandler->Store(parsedMsg.record());
            return std::string("created"); // TODO: think about various status responses type from server to client
        }
        case gpb::Message_Action::Message_Action_READ: {
            return storageHandler->Read(parsedMsg.record());
        }

        case gpb::Message_Action::Message_Action_UPDATE: {
            storageHandler->Update(parsedMsg.record());
            return std::string("update"); // TODO: think about various status responses type from server to client
        }
        case gpb::Message_Action::Message_Action_DELETE: {
            storageHandler->Remove(parsedMsg.record());
            return std::string("deleted"); // TODO: think about various status responses type from server to client
        }
        case gpb::Message_Action::Message_Action_AUTHENTICATE: {
            user.isAuthenticated = storageHandler->Authenticate(parsedMsg.record());
            return user.isAuthenticated ? std::string("come on in")
                                        : std::string("You not shall pass"); // TODO: think about various status
                                                                             // responses type from server to client
        }
        default:
            break;
        }

        return std::string("unknown command");
    }

  private:
    std::shared_ptr<dataStorageHandler> storageHandler;
};

} // namespace Server
