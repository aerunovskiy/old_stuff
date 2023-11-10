#ifndef ALE_ERROR_HANDLER_H
#define ALE_ERROR_HANDLER_H

#include <string>

#include "vlog.h"

// Класс Error_Handler представляет собой обработчик ошибок
// В зависимости от параметра Reason метода DisconnectIndication
// класса TSAP, ErrorHanlder сообщает пользователю о причине
// разрыва соединения

class AleErrorHandler
{
public:
    // Метод логирует код ошибки и ее причину в соответствии с документацией
    // Subset-098 Table 11
    static void printAleError(uint16_t reason, uint16_t subreason)
    {
        vfatal << "ALE: Disconnect Reason" << reason << "SubReason" << subreason;
        switch (reason)
        {
            case 0:
            {
                switch (subreason)
                {
                    case 0:
                        vfatal << "Normal Release";
                        break;
                }
                break;
            }

            case 1:
            {
                vfatal << "Network Error";
                switch (subreason)
                {
                    case 1:
                        vfatal << "Number not assigned; Invalid number format";
                        break;

                    case 2:
                        vfatal << "Channel QoS is unacceptable";
                        break;

                    case 3:
                        vfatal << "Impossible to establish physical connection"
                                << "for other reasons";
                        break;

                    case 4:
                        vfatal << "Address information incompatible with QoS requested";
                        break;
                }
                break;
            }

            case 2:
            {
                vfatal << "Network resource not available";
                switch (subreason)
                {
                    case 1:
                        vfatal << "No channel available";
                        break;

                    case 2:
                        vfatal << "Network congestion";
                        break;

                    case 3:
                        vfatal << "Other sub-reason";
                        break;
                }
                break;
            }

            case 3:
            {
                vfatal << "Service or option is temporarily not available";
                switch (subreason)
                {
                    case 1:
                        vfatal << "QoS not available";
                        break;

                    case 2:
                        vfatal << "Bearer capability not available";
                        break;
                }
                break;
            }

            case 5:
            {
                switch (subreason)
                {
                    case 0:
                        vfatal << "Reason unknown";
                        break;
                }
                break;
            }

            case 6:
            {
                vfatal << "Unsupported Application";
                switch (subreason)
                {
                    case 1:
                        vfatal << "Requested Application type is not supported"
                                << "at the called address";
                        break;

                    case 2:
                        vfatal << "Called user not known (e.g. No reply)";
                        break;

                    case 3:
                        vfatal << "Called user not available (e.g. Busy)";
                        break;
                }
                break;
            }

            case 7:
            {
                vfatal << "Internal Error";
                switch (subreason)
                {
                    case 1:
                        vfatal << "Mandatory element (e.g. primitive parameter)"
                                << "is missing";
                        break;

                    case 2:
                        vfatal << "Inappropriate state";
                        break;

                    case 3:
                        vfatal << "Other sub-reasons";
                        break;
                }
                break;
            }
        }
    }
};

#endif // ALE_ERROR_HANDLER_H
