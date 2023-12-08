/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_things.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adardour <adardour@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/06 19:50:44 by adardour          #+#    #+#             */
/*   Updated: 2023/12/08 18:42:41 by adardour         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http.server.hpp"

// void    print_nested_location(std::vector<LocationsBlock>& nestedLocation)
// {
//     if (!nestedLocation[0].getNestedLocations())
//     {
//         printf("ko\n");
//     }
//     else
//     {
//         printf("nested location %lu\n",nestedLocation[0].getNestedLocations()->size());

//     }
// }

void    print_location(std::vector<LocationsBlock> &locations)
{
    int i = 0;
    while (i < locations.size())
    {   
        printf("location %d\t",i);
        printf("path %s\n",locations[i].getPath().c_str());
        print_dir(locations[i].getDirective());
        i++;
    }
}

void    print_args(std::vector<std::string> &args)
{
    int i = 0;
    while (i < args.size())
    {
        printf("%s\t",args[i].c_str());
        i++;
    }
    
}
void    print_dir(std::vector<Directives> &dir)
{   
    int i = 0;
   while (i < dir.size())
   {
        printf("directive name %s\t",dir[i].getDirective().c_str());
        print_args(dir[i].getArgument());
        printf("\n");
        i++;
   }
}

void    print_server(std::vector<ServerBlocks> &serverBlocks)
{
    int index = 0;
    
    while (index < serverBlocks.size())
    {
        printf("Server %d ================================== \n\n",index);
        print_dir(serverBlocks[index].getDirectives());
        printf("\n");
        print_location(serverBlocks[index].getLocations());
        printf("\n");
        index++;
        printf("===========================================\n\n");
    }
}