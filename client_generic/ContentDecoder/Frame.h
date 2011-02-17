///////////////////////////////////////////////////////////////////////////////
//
//    electricsheep for windows - collaborative screensaver
//    Copyright 2003 Nicholas Long <nlong@cox.net>
//	  electricsheep for windows is based of software
//	  written by Scott Draves <source@electricsheep.org>
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef	_FRAME_H_
#define	_FRAME_H_

#include	"base.h"
#include	"SmartPtr.h"
#include	"linkpool.h"
#include	"AlignedBuffer.h"

namespace ContentDecoder
{

struct sMetaData
{
	fp4 m_Fade;
	std::string m_FileName;
	uint32 m_SheepID;
	uint32 m_SheepGeneration;
	time_t m_LastAccessTime;
	bool m_IsEdge;
};

/*
	CVideoFrame.
	Base class for a decoded video frame.
	Will converts itself to specified format if needed.
*/
class CVideoFrame
{
	protected:
		uint32	m_Width;
		uint32	m_Height;
		fp8		m_Pts;
		
		sMetaData m_MetaData;

		Base::spCAlignedBuffer m_spBuffer;
		AVFrame		*m_pFrame;

	public:
		CVideoFrame( AVCodecContext *_pCodecContext, PixelFormat _format, std::string _filename ) : m_pFrame(NULL)
			{
				assert( _pCodecContext );
				if ( _pCodecContext == NULL)
					g_Log->Info( "_pCodecContext == NULL" );

				m_MetaData.m_Fade = 1.f;
				m_MetaData.m_FileName = _filename;
				m_MetaData.m_LastAccessTime = 0;
				m_MetaData.m_SheepID = 0;
				m_MetaData.m_SheepGeneration = 0;
				m_MetaData.m_IsEdge = false;

				m_Width = _pCodecContext->width;
				m_Height = _pCodecContext->height;

				m_pFrame = avcodec_alloc_frame();
				if (m_pFrame != NULL)
				{
					int32 numBytes = avpicture_get_size( _format, _pCodecContext->width, _pCodecContext->height );
					m_spBuffer = new Base::CAlignedBuffer( numBytes * sizeof(uint8) );
					avpicture_fill( (AVPicture *)m_pFrame, m_spBuffer->GetBufferPtr(), _format, _pCodecContext->width, _pCodecContext->height );
				} else
					g_Log->Error( "m_pFrame == NULL" );
			}

			virtual ~CVideoFrame()
			{
				if( m_pFrame )
				{
					av_free( m_pFrame );
				}
			}

			inline void GetMetaData(sMetaData &_metadata)
			{
				_metadata = m_MetaData;
			}

			inline void SetMetaData_Fade(fp4 _fade)
			{
				m_MetaData.m_Fade = _fade;
			}

			inline void SetMetaData_FileName(std::string _filename)
			{
				m_MetaData.m_FileName = _filename;
			}
			
			inline void SetMetaData_SheepID(uint32 _sheepid)
			{
				m_MetaData.m_SheepID = _sheepid;
			}

			inline void SetMetaData_SheepGeneration(uint32 _sheepgeneration)
			{
				m_MetaData.m_SheepGeneration = _sheepgeneration;
			}

			inline void SetMetaData_IsEdge(bool _isedge)
			{
				m_MetaData.m_IsEdge = _isedge;
			}

			inline void SetMetaData_atime(time_t _atime)
			{
				m_MetaData.m_LastAccessTime = _atime;
			}

			inline	void	Pts( const fp8 _pts )			{	m_Pts = _pts;		};
			inline	fp8		Pts( void )						{	return m_Pts;		};
			inline	uint32	Width()							{	return m_Width;		};
			inline	uint32	Height()						{	return m_Height;	};

			inline	AVFrame	*Frame()	{	return m_pFrame;	};

			virtual inline uint8		*Data()
			{
				if( !m_pFrame )
					return NULL;

				return m_pFrame->data[0];
			};
			
			virtual inline Base::spCAlignedBuffer& StorageBuffer()
			{
				return m_spBuffer;
			};

			virtual inline const int32	Stride()
			{
				if( !m_pFrame )
					return 0;

				return m_pFrame->linesize[0];
			};

			//POOLED( CVideoFrame, Memory::CLinkPool );
};

MakeSmartPointers( CVideoFrame );

}

#endif