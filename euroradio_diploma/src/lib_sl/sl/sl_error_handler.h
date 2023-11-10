#ifndef SL_ERROR_HANDLER_H
#define SL_ERROR_HANDLER_H

#include <string>

#include "vlog.h"

// Класс Error_Handler представляет собой обработчик ошибок
// В зависимости от параметра Reason метода DisconnectIndication
// класса SL, ErrorHanlder сообщает пользователю о причине
// разрыва соединения

class SlErrorHandler
{
public:
    static void printSlError(uint8_t reason, uint8_t subreason)
    {
        vfatal  << "SL: Error Reason"   << static_cast<int>(reason)
                << "SubReason"          << static_cast<int>(subreason);

        switch (reason)
        {
            case 0:
            {
                switch (subreason)
                {
                    case 0:
                        vfatal << "Normal release requested by peer SFM user";
                        break;

                    default:
                        vfatal << "Unregistered reason code";
                        break;
                }
                break;
            }

            case 1:
            {
                vfatal << "No transport service available";
                switch (subreason)
                {
                    case 1:
                        vfatal << "Network Error";
                        break;

                    case 2:
                        vfatal << "Network resource not available";
                        break;

                    case 3:
                        vfatal << "Service or option is temporarily not available";
                        break;

                    case 5:
                        vfatal << "Reason unknown";
                        break;

                    case 6:
                        vfatal << "Called TS user not available";
                        break;

                    case 8:
                        vfatal << "No Mobile Termination has been registered";
                        break;

                    default:
                        vfatal << "Unregistered reason code";
                        break;
                }
                break;
            }

            case 3:
            {
                vfatal << "Missing parameter or invalid parameter value";
                switch (subreason)
                {
                    case 2:
                        vfatal << "Missing authentication key";
                        break;

                    case 3:
                        vfatal  << "Other problem related to the key management"
                                << "(e.g. loss of session key).";
                        break;

                    case 4:
                        vfatal << "Authentication key not currently valid";
                        break;

                    case 29:
                        vfatal << "Requested safety feature is not supported";
                        break;

                    default:
                        vfatal << "Unregistered reason code";
                        break;
                }
                break;
            }

            case 4:
            {
                vfatal << "Invalid MAC";
                switch (subreason)
                {
                    case 1:
                        vfatal << "MAC error";
                        break;

                    case 2:
                               vfatal << "MAC error in AU2 SaPDU";
                        break;

                    case 3:
                        vfatal << "MAC error in AU3 SaPDU";
                        break;

                    case 4:
                        vfatal << "MAC error in AR SaPDU";
                        break;

                    default:
                        vfatal << "Unregistered reason code";
                        break;
                }
                break;
            }

            case 5:
            {
                switch (subreason)
                {
                    case 1:
                        vfatal  << "Failure in sequence integrity."
                                << "Replay of authentication message (AU1 SaPDU,"
                                << "AU2 SaPDU, AU3 SaPDU, AR SaPDU) after"
                                << "connection establishment. Error code is used, if"
                                << "the error is not covered by reason code 9.";
                        break;

                    default:
                        vfatal << "Unregistered reason code";
                        break;
                }
                break;
            }

            case 6:
            {
                vfatal << "Failure in the direction flag";
                switch (subreason)
                {
                    case 1:
                        vfatal << "Value of direction flag '0' instead of '1'";
                        break;

                    case 2:
                        vfatal << "Value of direction flag '1' instead of '0'";
                        break;

                    default:
                        vfatal << "Unregistered reason code";
                        break;
                }
                break;
            }

            case 7:
            {
                vfatal << "Timeout at connection establishment";
                switch (subreason)
                {
                    case 3:
                        vfatal << "Timeout of Testab without receiving the AR SaPDU";
                        break;

                    default:
                        vfatal << "Unregistered reason code";
                        break;
                }
                break;
            }

            case 8:
            {
                vfatal << "Invalid SaPDU field";
                switch (subreason)
                {
                    case 1:
                        vfatal << "Invalid information field";
                        break;

                    case 4:
                        vfatal  << "Invalid responding ETCSID in AU2, i.e. ETCS-Identity"
                                << "does not correspond to an acceptable ETCS ID";
                        break;

                    case 5:
                        vfatal  << "Invalid AU1 SaPDU : the header indicates a AU1 SaPDU"
                                << "but the rest of the SaPDU does not match with the"
                                << "structure of an AU1 SaPDU";
                        break;

                    default:
                        vfatal << "Unregistered reason code";
                        break;
                }
                break;
            }

            case 9:
            {
                vfatal << "Failure in sequence of the SaPDUs during connection set-up";
                switch (subreason)
                {
                    case 1:
                        vfatal  << "Transmission of AU1 SaPDU but a message"
                                << "different from AU2 SaPDU is obtained";
                        break;

                    case 2:
                        vfatal  << "Transmission of AU2 SaPDU but a message"
                                << "different from AU3 SaPDU is obtained";
                        break;

                    case 3:
                        vfatal  << "Transmission of AU3 SaPDU but a message"
                                << "different from AR SaPDU is obtained";
                        break;

                    default:
                        vfatal << "Unregistered reason code";
                        break;
                }
                break;
            }

            case 10:
            {
                vfatal << "SaPDU length error";
                switch (subreason)
                {
                    case 1:
                        vfatal << "AU1 SaPDU length error";
                        break;

                    case 2:
                        vfatal << "AU2 SaPDU length error";
                        break;

                    case 3:
                        vfatal << "AU3 SaPDU length error";
                        break;

                    case 5:
                        vfatal << "DT SaPDU length error";
                        break;

                    case 8:
                        vfatal << "AR SaPDU length error";
                        break;

                    default:
                        vfatal << "Unregistered reason code";
                        break;
                }
                break;
            }

            case 128:
            {
                vfatal << "REASON_SFM_INTERNAL";
                switch (subreason)
                {
                    case 1:
                        vfatal << "SUBREASON_NO_CONTACT_WITH_COM_SERVER";
                        break;

                    case 2:
                        vfatal << "SUBREASON_ERROR_IN_MESS_FROM_CMM";
                        break;

                    case 3:
                        vfatal << "SUBREASON_CONNECT_DENIED_NO_MORE_SAFE_CONNECTIONS";
                        break;

                    case 4:
                        vfatal << "SUBREASON_KEY_MISSING";
                        break;

                    case 5:
                        vfatal << "SUBREASON_KEY_INVALID_TIME";
                        break;

                    case 6:
                        vfatal << "SUBREASON_KEY_FILE_ERROR";
                        break;

                    case 7:
                        vfatal << "SUBREASON_KEY_CRC_ERROR";
                        break;

                    case 8:
                        vfatal << "SUBREASON_KEY_TIME_ERROR";
                        break;

                    default:
                        vfatal << "Unregistered reason code";
                        break;
                }
                break;
            }

            default:
                vfatal << "Unregistered reason code";
                break;
        }
    }
};


#endif // SL_ERROR_HANDLER_H
