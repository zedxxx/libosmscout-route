#!/usr/bin/env python

import io


def is_stop_word_found(line):

    stop_words = ('IGNORE', 'PATH', 'ROUTE', 'ROUTE_MASTER')
    skip_list = ('IGNORESEALAND', )

    for s in line.split(' '):
        skip_word = False

        for w in skip_list:
            if s.find(w) >= 0:
                skip_word = True
                break

        if skip_word:
            continue

        for w in stop_words:
            if s.find(w) >= 0:
                return True

    return False


def process_type(t):

    do_ignore = True

    for line in t:
        # remove <whitespaces>
        line = line.strip()
        # remove comments
        i = line.find('//')
        if i >= 0:
            line = line[:i]
        # detect words-markers
        if is_stop_word_found(line):
            do_ignore = False
            break

    if do_ignore:
        line = t[0]
        if line.find('//') >= 0:
            line = line.replace('//', 'IGNORE // not-a-route //')
        else:
            line = line[:-1] + ' IGNORE // not-a-route\n'
        t[0] = line

    return ''.join(t)


TYPES_NOT_FOUND = 0
TYPES_FOUND = 1
TYPES_END = 2


def make_route_ost(ost, route):

    with io.open(route, 'w', encoding='utf-8', newline='\n') as f:

        types = TYPES_NOT_FOUND
        type_in = False
        t = []

        for line in open(ost):

            if types == TYPES_NOT_FOUND and line.startswith('TYPES'):
                types = TYPES_FOUND
            elif types == TYPES_FOUND and line.find('DO NOT CHANGE') > 0:
                types = TYPES_END

            if types == TYPES_FOUND:
                if line.startswith('  TYPE '):
                    if type_in:
                        s = process_type(t)
                        f.write(s.decode('utf-8'))
                    type_in = True
                    del t[:]
                elif type_in and line.startswith(('\n', 'END')):
                    type_in = False
                    line = process_type(t) + line
            elif type_in:
                s = process_type(t)
                f.write(s.decode('utf-8'))
                type_in = False

            if type_in:
                t.append(line)
            else:
                f.write(line.decode('utf-8'))


if __name__ == '__main__':
    make_route_ost('./stylesheets/map.ost', './stylesheets/route.ost')
