/* === This file is part of Calamares - <https://calamares.io> ===
 *
 *   SPDX-FileCopyrightText: 2013-2016 Teo Mrnjavac <teo@kde.org>
 *   SPDX-FileCopyrightText: 2018 Adriaan de Groot <groot@kde.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 *   Originally from Tomahawk, portions:
 *   SPDX-FileCopyrightText: 2010-2011 Christian Muehlhaeuser <muesli@tomahawk-player.org>
 *   SPDX-FileCopyrightText: 2010-2011 Leo Franchi <lfranchi@kde.org>
 *   SPDX-FileCopyrightText: 2010-2012 Jeff Mitchell <jeff@tomahawk-player.org>
 *
 *   Calamares is Free Software: see the License-Identifier above.
 *
 *
 */

#include "String.h"
#include "Logger.h"

#include <QStringList>

namespace CalamaresUtils
{
QString
removeDiacritics( const QString& string )
{
    // *INDENT-OFF*
    // clang-format off
    // Map these extended-Latin characters to ASCII; keep the
    // layout so that one line in *diacriticLetters* corresponds
    // to one line of replacements in *noDiacriticLetters*.
    static const QString diacriticLetters = QString::fromUtf8(
        "ŠŒŽšœžŸ¥µÀ"
        "ÁÂÃÄÅÆÇÈÉÊ"
        "ËÌÍÎÏÐÑÒÓÔ"
        "ÕÖØÙÚÛÜÝßà"
        "áâãäåæçèéê"
        "ëìíîïðñòóô"
        "õöøùúûüýÿÞ"
        "þČčĆćĐđŠšŽ"
        "žŞşĞğİıȚțȘ"
        "șĂăŐőŰűŘřĀ"
        "āĒēĪīŌōŪūŢ"
        "ţẀẁẂẃŴŵŶŷĎ"
        "ďĚěŇňŤťŮůŔ"
        "ŕĄąĘęŁłŃńŚ"
        "śŹźŻż"
    );
    static const QStringList noDiacriticLetters = {
        "S", "OE", "Z", "s", "oe", "z", "Y", "Y", "u", "A",
        "A", "A", "A", "A", "AA", "AE", "C", "E", "E", "E",
        "E", "I", "I", "I", "I", "D", "N", "O", "O", "O",
        "O", "E", "OE", "U", "U", "U", "E", "Y", "s", "a",
        "a", "a", "a", "e", "aa", "ae", "c", "e", "e", "e",
        "e", "i", "i", "i", "i", "d", "n", "o", "o", "o",
        "o", "e", "oe", "u", "u", "u", "e", "y", "y", "TH",
        "th", "C", "c", "C", "c", "DJ", "dj", "S", "s", "Z",
        "z", "S", "s", "G", "g", "I", "i", "T", "t", "S",
        "s", "A", "a", "O", "o", "U", "u", "R", "r", "A",
        "a", "E", "e", "I", "i", "O", "o", "U", "u", "T",
        "t", "W", "w", "W", "w", "W", "w", "Y", "y", "D",
        "d", "E", "e", "N", "n", "T", "t", "U", "u", "R",
        "r", "A", "a", "E", "e", "L", "l", "N", "n", "S",
        "s", "Z", "z", "Z", "z"
    };
    // clang-format on
    // *INDENT-ON*

    QString output;
    for ( const QChar& c : string )
    {
        int i = diacriticLetters.indexOf( c );
        if ( i < 0 )
        {
            output.append( c );
        }
        else
        {
            QString replacement = noDiacriticLetters[ i ];
            output.append( replacement );
        }
    }

    return output;
}


// Function CalamaresUtils::obscure based on KStringHandler::obscure,
// part of KDElibs by KDE, file kstringhandler.cpp.
// Original copyright statement follows.
/* This file is part of the KDE libraries
   Copyright (C) 1999 Ian Zepp (icszepp@islc.net)
   Copyright (C) 2006 by Dominic Battre <dominic@battre.de>
   Copyright (C) 2006 by Martin Pool <mbp@canonical.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/
QString
obscure( const QString& string )
{
    QString result;
    const QChar* unicode = string.unicode();
    for ( int i = 0; i < string.length(); ++i )
    // yes, no typo. can't encode ' ' or '!' because
    // they're the unicode BOM. stupid scrambling. stupid.
    {
        result += ( unicode[ i ].unicode() <= 0x21 ) ? unicode[ i ] : QChar( 0x1001F - unicode[ i ].unicode() );
    }
    return result;
}


QString
truncateMultiLine( const QString& string, CalamaresUtils::LinesStartEnd lines, CalamaresUtils::CharCount chars )
{
    const char NEWLINE = '\n';
    const int maxLines = lines.atStart + lines.atEnd;
    if ( maxLines < 1 )
    {
        QString shorter( string );
        shorter.truncate( chars.total );
        return shorter;
    }

    const int linesInString = string.count( NEWLINE ) + ( string.endsWith( NEWLINE ) ? 0 : 1 );
    if ( ( string.length() <= chars.total ) && ( linesInString <= maxLines ) )
    {
        return string;
    }

    QString front, back;
    if ( string.count( NEWLINE ) >= maxLines )
    {
        int from = -1;
        for ( int i = 0; i < lines.atStart; ++i )
        {
            from = string.indexOf( NEWLINE, from + 1 );
            if ( from < 0 )
            {
                // That's strange, we counted at least maxLines newlines before
                break;
            }
        }
        if ( from > 0 )
        {
            front = string.left( from + 1 );
        }

        int lastNewLine = -1;
        int lastCount = string.endsWith( NEWLINE ) ? -1 : 0;
        for ( auto i = string.rbegin(); i != string.rend() && lastCount < lines.atEnd; ++i )
        {
            if ( *i == NEWLINE )
            {
                ++lastCount;
                lastNewLine = int( i - string.rbegin() );
            }
        }
        if ( ( lastNewLine >= 0 ) && ( lastCount >= lines.atEnd ) )
        {
            back = string.right( lastNewLine );
        }
    }

    return front + back;
}


}  // namespace CalamaresUtils
