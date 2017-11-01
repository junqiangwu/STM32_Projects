//
// Comment the selected block of text using single line comments and indent it
//
macro CommentBlock()
{
	hbuf = GetCurrentBuf();
	hwnd = GetCurrentWnd();

	sel = GetWndSel(hwnd);

	iLine = sel.lnFirst;
	
	while (iLine <= sel.lnLast)
	{
		szLine = GetBufLine(hbuf, iLine);
		szLine = cat("//	", szLine);
		PutBufLine(hbuf, iLine, szLine);
		iLine = iLine + 1;
	}

	if (sel.lnFirst == sel.lnLast)
	{
		tabSize = _tsGetTabSize() - 1;
		sel.ichFirst = sel.ichFirst + tabSize;
		sel.ichLim = sel.ichLim + tabSize;
	}
	SetWndSel(hwnd, sel);
}


//
// Undo the CommentBlock for the selected text.
//
macro UnCommentBlock()
{
	hbuf = GetCurrentBuf();
	hwnd = GetCurrentWnd();

	sel = GetWndSel(hwnd);

	iLine = sel.lnFirst;


	tabSize = 0;
	while (iLine <= sel.lnLast)
	{
		szLine = GetBufLine(hbuf, iLine);
		len = strlen(szLine);
		szNewLine = "";
		if (len > 1)
		{
			if (szLine[0] == "/" && szLine[1] == "/")
			{
				if (len > 2)
				{
					if (AsciiFromChar(szLine[2]) == 9)
					{
						tabSize = _tsGetTabSize() - 1;
						szNewLine = strmid(szLine, 3, strlen(szLine));
					}
				}

				if (szNewLine == "")
				{
					szNewLine = strmid(szLine, 2, strlen(szLine));
					tabSize = 2;
				}
				
				PutBufLine(hbuf, iLine, szNewLine);
			}
		}
		iLine = iLine + 1;
	}

	if (sel.lnFirst == sel.lnLast)
	{
		sel.ichFirst = sel.ichFirst - tabSize;
		sel.ichLim = sel.ichLim - tabSize;
	}

	SetWndSel(hwnd, sel);
}



macro _tsGetTabSize()
{
	szTabSize = GetReg("TabSize");

	if (szTabSize != "")
	{
		tabSize = AsciiFromChar(szTabSize[0]) - AsciiFromChar("0");
	}
	else
	{
		tabSize = 4;
	}

	return tabSize;
}




//
// Reformat a selected comment block to wrap text at 80 columns.
// The start of the selection (upper left most character of the selection) is
// handled specially, in that it specifies the left most column at which all
// lines will begin.  For example, if the following block was selected starting
// at the @ symbol, through the last line of the block...
//------------------------------------------------------------------------------
// preamble:	@ This is a line that will be wrapped keeping the "at" symbol in its current column.
// All lines following it that are selected will use that as their starting column.  See below to see how the wrapping
// works for this block of text.
//------------------------------------------------------------------------------
// preamble:	@ This is a line that will be wrapped keeping the "at" symbol in
//				its current column. All lines following it that are selected
//				will use that as their starting column.  See below to see how
//				the wrapping works for this block of text.
//
macro tsReformatCommentBlock()
{
	hbuf = GetCurrentBuf();
	hwnd = GetCurrentWnd();

	sel = GetWndSel(hwnd);

	tabSize = _tsGetTabSize();
	leftTextCol = 0 - 1;
	colWrap = 80;

	// Find the starting column, and create a Margin string
	ichFirst = sel.ichFirst;
	
	// Single line comment reformat?
	if (sel.ichFirst == sel.ichLim && sel.lnFirst == sel.lnLast)
	{
		ichFirst = 0;
	}

	rec = _tsGetStartColumn(hbuf, ichFirst, sel.lnFirst);

	if (rec == "")
		stop;

	colLeftMargin = rec.colMargin;
	szMargin = "";

	colComment = 0;
	if (rec.colComment >= 0)
	{
		colComment = rec.colComment + 2
		szMargin = _tsAddWhiteToColumn(szMargin, 0, rec.colComment, tabSize);
		szMargin = cat(szMargin, "//");
	}

	szMargin = _tsAddWhiteToColumn(szMargin, colComment, rec.colMargin, tabSize);

	rec = "";

	szCurMargin = "";
	if (ichFirst != 0)
	{
		szLine = GetBufLine(hbuf, sel.lnFirst);
		szCurMargin = strmid(szLine, 0, ichFirst);
	}
	else
	{
		szCurMargin = szMargin;
		szMargin = "";
	}

	insertLine = sel.lnFirst;
	iLine = sel.lnFirst;
	szRemainder = "";
	while (1)
	{
//		msg("$0-" # iLine # ":" # szRemainder);
		rec = _tsGetNextCommentString(hbuf, ichFirst, szRemainder, iLine, sel.lnLast, colWrap);
		ichFirst = 0;

		if (rec == "")
			break;

//		msg("$1-" # rec.ln # ":" # rec.szComment);
		szLine = rec.szComment;
	
		ich = 0;
		col = colLeftMargin;
		len = strlen(szLine);
		
		ichPrevCharToWhite = 0-1;
		ichPrevWhiteToChar = 0-1;
//		msg("Leftovers @szRemainder@");

		while (ich < len)
		{
			if (AsciiFromChar(szLine[ich]) == 9)
			{
				col = (((col + tabSize) / tabSize) * tabSize);
			}
			else
			{
				col = col + 1;
			}

			if (col > colWrap)
				break;

			fIsWhitespace = _tsIsWhitespaceChar(szLine[ich]);
			fIsWhitespace1 = 1;

			if (ich + 1 < len)
			{
				fIsWhitespace1 = _tsIsWhitespaceChar(szLine[ich + 1]);
			}

			if (!fIsWhitespace && fIsWhitespace1)
				ichPrevCharToWhite = ich;
			
			ich = ich + 1;
		}

		if (ichPrevCharToWhite > 0)
		{
//			msg("$2:" # strmid(szLine, 0, ichPrevCharToWhite + 1));
			ich = ichPrevCharToWhite + 1;

			while (ich < len)
			{
				if (!_tsIsWhitespaceChar(szLine[ich]))
				{
					ichPrevWhiteToChar = ich - 1;
//					msg("$3:" # strmid(szLine, ichPrevWhiteToChar + 1, len));
					break;
				}
				ich = ich + 1;
			}
		}

		if (ichPrevCharToWhite > 0 && col > colWrap)
		{
			szNewLine = cat(szCurMargin, strmid(szLine, 0, ichPrevCharToWhite + 1));
			szRemainder = "";
			if (ichPrevWhiteToChar > 0)
				szRemainder = strmid(szLine, ichPrevWhiteToChar + 1, len);
				
			if (ichPrevCharToWhite > ichPrevWhiteToChar)
				msg("!!!Wrap, duplicating word " # ichPrevWhiteToChar # " " # ichPrevCharToWhite # " " # szNewLine # " >>> " # szRemainder);
//				msg(szLine);
//				msg(col # " " # ichPrevWhiteToChar # " " # ichPrevCharToWhite # " " # szNewLine # " >>> " # szRemainder);
		}
		else if (szLine != "")
		{
			szNewLine = cat(szCurMargin, szLine );
			szRemainder = "";
//			sel.lnLast = sel.lnLast + 1;
		}

		iLine = rec.ln;
		if (insertLine == iLine)
		{
			iLine = iLine + 1;
			sel.lnLast = sel.lnLast + 1;
			
//			msg("$5-" # insertLine # ":" # szNewLine);
			InsBufLine(hbuf, insertLine, szNewLine);
		}
		else
		{
			szLine = GetBufLine(hbuf, insertLine);
			if (szLine != szNewLine)
			{
//				msg("$6-" # insertLine # ":" # szNewLine);
				PutBufLine(hbuf, insertLine, szNewLine);
			}
		}
		insertLine = insertLine + 1;

		if (szMargin != "")
		{
			szCurMargin = szMargin;
			szMargin = "";
		}
	}

	while (insertLine <= sel.lnLast)
	{
		DelBufLine(hbuf, insertLine);
		sel.lnLast = sel.lnLast - 1;
	}

	len = GetBufLineLength(hbuf, insertLine-1);

	sel.ichFirst = len;
	sel.ichLim = len;
	sel.lnFirst = sel.lnLast;
	SetWndSel(hwnd, sel);
}


macro _tsAddWhiteToColumn(sz, col0, col, tabSize)
{
	szTabs = "																														";
	szSpaces = "                ";

	tabs0 = col0 / tabSize;
	tabs = (col / tabSize) - tabs0;

	if (tabs == 0)
		foo = col0;
	else
		foo = (tabs + tabs0) * tabSize;
	spaces = col - foo;
//	msg(col0 # " " # col # " " # tabs # " " # spaces # " " # tabs0);

	if (tabs)
		sz = cat(sz, strmid(szTabs, 0, tabs));

	if (spaces)
		sz = cat(sz, strmid(szSpaces, 0, spaces));

	return sz;
}

macro _tsGetStartColumn(hbuf, ichBegin, ln)
{
	szLine = GetBufLine(hbuf, ln);
	len = strlen(szLine);
	tabSize = _tsGetTabSize();
	ich = 0;

	colMargin = 0;
	colComment = 0-1;

	rec = "";
	rec.colMargin = colMargin;
	rec.colComment = colComment;
	
	while (ich < len)
	{
		if (AsciiFromChar(szLine[ich]) == 9)
		{
			colMargin = (((colMargin + tabSize) / tabSize) * tabSize);
		}
		else
		{
			colMargin = colMargin + 1;
		}

		if (colComment < 0)
		{
			if (ich + 1 < len)
			{
				if (szLine[ich] == "/" && szLine[ich+1] == "/")
				{
					colComment = colMargin - 1;
					ich = ich + 2;
					colMargin = colMargin + 1;
					continue;
				}
			}
		}

		if (ich >= ichBegin)
		{
			if (!_tsIsWhitespaceChar(szLine[ich]))
			{
				rec.colMargin = colMargin - 1;
				rec.colComment = colComment;
//				msg(szLine[ich]);
				return rec;
			}
		}

		ich = ich + 1;
	}
	
	return rec;
}

macro _tsGetNextCommentString(hbuf, ichSkip, szRemainder, ln, lnLast, colWrap)
{
	rec = "";

	// Go until we get a string that is at least long enough to fill a line
	// or, we run out of lines.
	if (szRemainder == "" && ln > lnLast)
		return "";
		
	ichFirst = ichSkip;
//	msg(ichSkip);
	while (1)
	{
		if (ln > lnLast)
		{
			rec.szComment = szRemainder;
			rec.ln = ln;
			return rec;
		}

		cchRemainder = strlen(szRemainder);

		if (cchRemainder > colWrap)
		{
			rec.szComment = szRemainder;
			rec.ln = ln;
			return rec;
		}

		szLine = GetBufLine(hbuf, ln);
		len = strlen(szLine);

		if (ichSkip == 0)
			ichFirst = _tsSkipPastCommentAndWhitespace(szLine, len);
		ichSkip = 0;
			
		ichLast = len - 1;

		// Now, strip out all whitespace at the end of the line
		while (ichLast >= ichFirst)
		{
			if (!_tsIsWhitespaceChar(szLine[ichLast]))
			{
				break;
			}
			ichLast = ichLast - 1;
		}

		// Entire line is whitespace?
		if (ichLast < ichFirst)
		{
			if (szRemainder == "")
				ln = ln + 1;
			rec.szComment = szRemainder;
			rec.ln = ln;
			return rec;

		}
		// length of the non whitespaced comment + 1 space + cchRemainder
		if ((ichLast + 1) - ichFirst + cchRemainder + 1 > 255)
		{
			// It may not format the current line quite right, but
			// but at least we won't throw away some of the comment.
			rec.szComment = szRemainder;
			rec.ln = ln;
			return rec;
		}

		if (szRemainder != "")
			szRemainder = cat(szRemainder, " ");
		szRemainder = cat(szRemainder, strmid(szLine, ichFirst, ichLast + 1));
		ln = ln + 1;
	}
}



macro _tsSkipPastCommentAndWhitespace(szLine, len)
{
	ichFirst = 0;
	// Skip past the comment initiator "//" if there is one.
	while (ichFirst < len)
	{
		if (ichFirst + 1 < len)
		{
			if (szLine[ichFirst] == "/" && szLine[ichFirst+1] == "/")
			{
				ichFirst = ichFirst + 2;
				break;
			}
		}
		ichFirst = ichFirst + 1;
	}

	// If no comment found in line, then start from the beginning
	if (ichFirst >= len)
		ichFirst = 0;

	ichFirst = ichFirst;
	// Now, strip out all whitespace after the comment start.
	while (ichFirst < len)
	{
		if (!_tsIsWhitespaceChar(szLine[ichFirst]))
		{
			break;
		}
		ichFirst = ichFirst + 1;
	}

	return ichFirst;
}
