/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abaioumy <abaioumy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/25 09:50:28 by abaioumy          #+#    #+#             */
/*   Updated: 2023/06/09 15:11:41 by abaioumy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"

int main( void )
{
    Server srv;
    Client cl;
    fd_set readfds;
    fd_set writefds;

    srv.createListenSocket();
    while ( true )
    {
        cl.setsManager( srv.getListenSocket(), readfds );
        if ( FD_ISSET( srv.getListenSocket(), &readfds ) )
        {
            struct ClientInfo *client = cl.getClient(-1, srv);
            if ( client->socket == -1 )
            {
                std::cerr << "accept() failed: " << strerror(errno) << std::endl;
                cl.deleteClient(client);
            }
            printf("New connection from: %s\n", cl.getAddress(client) );
        }
        cl.checkClients( readfds );
    }
    close(srv.getListenSocket());
}