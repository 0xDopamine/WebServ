/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbaioumy <mbaioumy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/12 18:42:15 by mbaioumy          #+#    #+#             */
/*   Updated: 2023/07/18 12:15:44 by mbaioumy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.hpp"
#include "configData.hpp"

Parser::Parser(): openingBraceCount(0), host_exists(false), status(OK), uploadExists(false) {};

Parser::~Parser() {};

bool	testExtension(char *argv) {

	std::string str(argv);

	if (str.find(".conf") != std::string::npos)
		return (false);
	return (true);
}

void    Parser::openFile(char *argv) {

	if (!argv)
		printError(NO_CONFIG_FILE);
	else if (testExtension(argv))
		printError(EXTENSION);
	else
	{
		std::ifstream   confFile(argv);
		readFile(confFile);
	}
} ;

bool    Parser::checkBracesError() {

	if (openingBraceCount == 0)
		return false;
	else
		return true;
}

std::string	Parser::cleanValue(std::string value) {

	size_t	pos = value.find_first_of(';');

	while (pos != std::string::npos) {
		value.erase(pos, 1);
		pos = value.find_first_of(';', pos);
	}
	return (value);
}

void	Parser::setServerContent(ServerSettings &server, int which, std::string value) {

	switch (which) {

		case PORT:
			if (host_exists == false)
			{
				if (value.size() - 1 > 0) {
					if (findSemicolon())
						server.setPort(cleanValue(value));
					else
						printError(SEMICOLON);
				}
				else
					printError(EMPTY);
			}
			else
				server.setPort(value);
			break ;
		case HOST:
			if (value.size() - 1 > 0) {
				if (findSemicolon())
					server.setHost(cleanValue(value));
				else
					printError(SEMICOLON);
			}
			else
				printError(EMPTY);
			break ;
		case NAME:
			if (value.size() - 1 > 0) {
				if (findSemicolon())    
					server.setName(cleanValue(value));
				else
					printError(SEMICOLON);
			}
			else
				printError(EMPTY);
			break ;
		case SIZE:
			if (value.size() - 1 > 0) {
				if (findSemicolon()) {
					std::stringstream ss(value);
					int size;
					ss >> size;
					server.setSize(size);
				}
				else
					printError(SEMICOLON);
			}
			else
				printError(EMPTY);
			break ;
		case UPLOAD:
			if (value.size() - 1 > 0) {
				if (findSemicolon()) {   
					server.setUpload(cleanValue(value));
					uploadExists = true;
				}
				else
					printError(SEMICOLON);
			}
			else
				printError(EMPTY);
			break ;
		case ERROR_PAGE:
			std::stringstream ss(value);
			std::string directive, status_code, path;
			
			ss >> directive >> status_code >> path;
			if (value.size() - 1 > 0) {
				if (findSemicolon())
					server.setErrorPages(status_code, cleanValue(path));
				else
					printError(SEMICOLON);
			}
			else
				printError(EMPTY);
			break ;
	}
}

int		countWords(std::string str) {

	std::stringstream ss(str);
	int	words = 0;
	std::string	word;

	while (ss >> word)
		words++;
	return (words);
}

void	Parser::setLocationContent(Location& location, int which, std::string value) {

	switch (which) {

		case ROOT:
			if (value.size() - 1 > 0) {	
				if (findSemicolon())
					location.setRoot(cleanValue(value));
				else
					printError(SEMICOLON);
			}
			else
				printError(EMPTY);
			break ;
		case INDEX:
			if (value.size() - 1 > 0) {
				if (findSemicolon())
					location.setIndex(cleanValue(value));
				else
					printError(SEMICOLON);
			}
			else
				printError(EMPTY);
			break ;
		case AUTOINDEX:
			if (value.size() - 1 > 0) {
				if (findSemicolon() && value.find("on") != std::string::npos)	
					location.setAutoIndex();
				else
					printError(SEMICOLON);
			}
			else
				printError(EMPTY);
			break ;
		case UPLOAD:
			if (value.size() - 1 > 0) {
				if (findSemicolon())
					location.setUpload(cleanValue(value));
				else
					printError(SEMICOLON);
			}
			else
				printError(EMPTY);
			break ;
		case RETURN:
			std::stringstream ss(value);
			std::string path;
			std::string	str;
			int			words = countWords(value);
			std::string	status_code;

			if (words == 2) {
				ss >> str >> status_code;
				if (status_code.size() - 1 > 0) {
					if (findSemicolon())
						location.setRedirection(cleanValue(status_code), "");
					else
						printError(SEMICOLON);				
				}
				else
					printError(EMPTY);
			}
			else if (words == 3) {	
				ss >> str >> status_code >> path;
				if (path.size() - 1 > 0) {
					if (findSemicolon())
						location.setRedirection(status_code, cleanValue(path));
					else
						printError(SEMICOLON);				
				}
				else
					printError(EMPTY);
			}
			else if (words > 3) {
				std::cerr << "Syntax Error: Return field is invalid, too many elements!" << std::endl;
				exit(1);
			}
			else {
				printError(EMPTY);
				break ;
			}
			break ;
	}
}

void   Parser::readFile(std::ifstream& confFile) {

	if (confFile.is_open())
	{
		while (getline(confFile, line))
		{
			if (line.find("#") != std::string::npos || line.empty())
				continue ;
			if (line.find("{") != std::string::npos)
				openingBraceCount++;
			else if (line.find("}") != std::string::npos)
				openingBraceCount--;
			if (line.find("server") != std::string::npos)
				parseServer(confFile);
		}
	}
	else
		printError(NO_CONFIG_FILE);
	if (checkBracesError())
		printError(CURLYBRACE);
} ;

void	Parser::serverValuesValidation(ServerSettings server) {

	std::string port = server.getPort();
	int			portInt;
	std::stringstream ss(port);

	ss >> portInt;
	if (port.size() == 0) {
		std::cerr << "Syntax Error: port not found!" << std::endl;
		exit(1);
	}
	if (server.getName().size() == 0) {	
		std::cerr << "Syntax Error: server name not found!" << std::endl;
		exit(1);
	}
	if (portInt == 0) {	
		std::cerr << "Syntax Error: port is invalid!" << std::endl;
		exit(1);
	}
	if (server.getLocations().size() == 0) {
		std::cerr << "Syntax Error: Location is undefined!" << std::endl;
		exit(1);
	}
}

void	Parser::parseServer(std::ifstream& confFile) {

	ServerSettings  server;
	Context context;
	std::string optionalVal;

	server.initErrorPages();
	while (getline(confFile, line)) {

		std::stringstream ss(line);
		ss >> directive >> value >> optionalVal;
		if (line.find("#") != std::string::npos || line.empty())
			continue ;
		if (line.find("{") != std::string::npos)
			openingBraceCount++;
		if (line.find("listen") != std::string::npos)
		{
			if (optionalVal.size())
				host_exists = true;
			setServerContent(server, PORT, value);
			if (host_exists)
				setServerContent(server, HOST, optionalVal);
		}
		else if (line.find("upload") != std::string::npos)
			setServerContent(server, UPLOAD, value);
		else if (line.find("server_name") != std::string::npos)
			setServerContent(server, NAME, value);
		else if (line.find("body_size") != std::string::npos)
			setServerContent(server, SIZE, value);
		else if (line.find("error_page") != std::string::npos)
			setServerContent(server, ERROR_PAGE, line);
		else if (line.find("location") != std::string::npos)
			parseLocation(confFile, server, value);              
		else if (line.find("}") != std::string::npos) {
			openingBraceCount--;
			context.setServer(server);
			parsedData.push_back(context);
			break ;
		}
	}
	serverValuesValidation(server);
}

bool	examinePath(std::string value) {

	if (value.find("/") != std::string::npos)
		return (true);
	return (false);
}

void	Parser::locationValuesValidation(Location location) {

	if (location.getValue().size() == 0 || !examinePath(location.getValue())) {
		std::cerr << "Syntax Error: location value not found or invalid!" << std::endl;
		exit(1);
	}
	if (location.getRoot().size() == 0 || !examinePath(location.getRoot())) {
		std::cerr << "Syntax Error: location root value not found or invalid!" << std::endl; 
		exit(1);
	}
	if (location.getIndex().size() == 0) {
		location.setIndex("HOMEPAGE");
	}
	if ((location.getRedirection().status_code.size() > 3 || location.getRedirection().status_code.size() < 3) && 
		location.getRedirection().status_code != "-1") {
		std::cerr << "Syntax Error: return status code is invalid!" << std::endl;
		exit(1);
	}
	std::stringstream ss(location.getRedirection().status_code);
	int				  code;
	
	ss >> code;
	if (code == 0) {
		std::cerr << "Syntax Error: return status code is invalid!" << std::endl;
		exit(1);
	}
}

void	Parser::parseLocation(std::ifstream& confFile, ServerSettings& server, std::string& value) {

	Location	location;
	
	location.setValue(value);
	while(getline(confFile, line) && status == OK) {

		std::stringstream ss(line);
		ss >> directive >> value;
		if (line.find("#") != std::string::npos || line.empty())
			continue ;
		if (line.find("{") != std::string::npos)
			openingBraceCount++;
		if (line.find("root") != std::string::npos)
			setLocationContent(location, ROOT, value);
		else if (line.find("index") != std::string::npos)
			setLocationContent(location, INDEX, value);
		else if (line.find("autoindex") != std::string::npos)
			setLocationContent(location, AUTOINDEX, value);
		else if (line.find("upload") != std::string::npos && uploadExists == false)
			setLocationContent(location, UPLOAD, value);
		else if (line.find("return") != std::string::npos)
			setLocationContent(location, RETURN, line);
		if (line[0] == '}') {
			openingBraceCount--;
			server.setLocations(location);
			break ;
		}
		else if (line.find("}") != std::string::npos) {
			openingBraceCount--;
			server.setLocations(location);
			break ;
		}
	}
	locationValuesValidation(location);
}

void	Parser::printData() {

	for (size_t i = 0; i < parsedData.size(); i++) {

		Context context;
		context = parsedData[i];

		ServerSettings  server;
		server = context.getServer();

		std::cout << std::endl;
		std::cout << "ServerSettings: " << std::endl;
		std::cout << "listen: " << server.getPort() << std::endl;
		std::cout << "host: " << server.getHost() << std::endl;
		std::cout << "name: " << server.getName() << std::endl;
		std::cout << "body size: " << server.getSize() << std::endl;
		std::cout << "upload: " << server.getUpload() << std::endl;
		
		std::cout << "Errors: " << std::endl;
		std::map<std::string, std::string> epMap = server.getErrorPages();
		std::map<std::string, std::string>::iterator itr;
		for(itr=epMap.begin();itr!=epMap.end();itr++)
		{
			std::cout << itr->first <<" " << itr->second << std::endl;
		}
		std::vector<Location>   locationVec = server.getLocations();
		for (size_t i = 0; i < locationVec.size(); i++) {

			std::cout << "locations: " << std::endl;
			std::cout << "value: " << locationVec[i].getValue() << std::endl;
			std::cout << "root: " << locationVec[i].getRoot() << std::endl;
			std::cout << "index: " << locationVec[i].getIndex() << std::endl;
			std::cout << "upload: " << locationVec[i].getUpload() << std::endl;
			std::cout << "return: " << locationVec[i].getRedirection().status_code; 
			std::cout << " " << locationVec[i].getRedirection().path << std::endl;
			if (locationVec[i].getAutoIndex() == ON)
				std::cout << "autoindex: on" << std::endl;
		}
	}
}

bool    Parser::findSemicolon() {

	if (line.find(";")  != std::string::npos)
		return (true);
	return (false);
}

void    Parser::printError(int which) {

	status = ERROR;
	switch(which) {

		case SEMICOLON:
			std::cout << "Syntax Error: could not find semicolon" << std::endl;
			break ;
		case CURLYBRACE:
			std::cout << "Syntax Error: Curly brace not found, or misalighned" << std::endl;
			break ;
		case UNKNOWN:
			std::cout << "Error: " << directive << " <- Unknown expression." << std::endl;
			break ;
		case EMPTY:
			std::cout << "Error: " << directive << " <- Directive can't have empty value!" << std::endl;
			break ;
		case EXTENSION:
			std::cout << "Error: Invalid extension!" << std::endl;
			break ;
		case NO_CONFIG_FILE:
			std::cout << "Error: config file not found or invalid!" << std::endl;
			break ;
	}
	exit(1);
}

std::vector<Context> Parser::getParsedData( void ) const
{
	return (parsedData);
}