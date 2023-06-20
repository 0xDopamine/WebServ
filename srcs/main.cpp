/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbaioumy <mbaioumy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/12 18:42:12 by mbaioumy          #+#    #+#             */
/*   Updated: 2023/06/19 14:44:04 by mbaioumy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include/parser.hpp"

int main(int argc, char **argv) {

    Parser  parser;

    parser.openFile(argv[1]);
    parser.printData();
    return 0;
}
