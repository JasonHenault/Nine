/*#include "Nine.hpp"
#include "FastAccess.hpp"

using namespace NCONSOLE;

void Console::RegisterCommand(const std::string& name, const Console::Functor& function) {
    _commands[name] = function;
}

void Console::SendChar(char c) {
    if (!_enabled)
        return;

    switch (c) {
        // Return
        case '\n' :
        case '\r' :
            if (!_current.empty()) {
                ProcessCurrent();
                _current.clear();
            }
            break;

        // Backspace
        case '\b' :
            if (!_current.empty())
                _current.erase(_current.size() - 1);
            break;


        default :
            _current += c;
            break;
    }
}

void CConsole::ProcessCurrent()
{
    // On récupère la commande
    std::string command;
    std::istringstream iss(c_current);
    iss >> command;

    // On recherche la commande dans la table des commandes
    CommandTable::iterator it = _commaSnds.find(command);

    // Si elle s'y trouve on appelle la fonction correspondante, sinon on génère une erreur
    if (it != _commands.end())
    {
        // Récupération des paramètres
        std::string params;
        std::getline(iss, params);

        // Appel du foncteur correspondant à la commande
        it->second(params);
    }
    else
    {
        // Erreur, commande inconnue...
    }
}*/
