#include "stdafx.h"
#include "Elements.h"

namespace NSPresentationEditor
{
	CString NSPresentationEditor::CTheme::ToXmlEditor(const CMetricInfo& oInfo)
	{
		NSPresentationEditor::CStringWriter oWriter;

		oWriter.WriteString(_T("<Theme>"));

		// colors ----
		oWriter.WriteString(_T("<Colors>"));
		
		size_t nCountColors = m_arColorScheme.size();
		for (size_t i = 0; i < nCountColors; ++i)
		{
			CString strFormat = _T("");
			LONG lColorValue = m_arColorScheme[i].GetLONG_RGB();
			strFormat.Format(_T(".color%u { color: #%06x; } .stroke%u { stroke:#%06x; } .fill%u { fill:#%06x; }\n"), 
				i + 1, lColorValue, i + 1, lColorValue, i + 1, lColorValue);
			oWriter.WriteString(strFormat);
		}

		oWriter.WriteString(_T("</Colors>"));
		// -----------
		
		// fonts -----
		oWriter.WriteString(_T("<Fonts>"));
		
		size_t nCountFonts = m_arFonts.size();
		//if (nCountFonts > 1)
		//	nCountFonts = 1;
		for (size_t i = 0; i < nCountFonts; ++i)
		{
			CString strFormat = _T("");

			strFormat.Format(_T(".font%u { font-family:%s;tmdocs_charset:%d;tmdocs_monospace:%d;tmdocs_panose:%s; }\n"),
				i + 1, m_arFonts[i].Name, m_arFonts[i].Charset, m_arFonts[i].Monospace, m_arFonts[i].Panose);

			oWriter.WriteString(strFormat);
		}

		oWriter.WriteString(_T("</Fonts>"));

		// -----------
		
		// fonts dublicate
		oWriter.WriteString(_T("<FontsDublicate>"));
		for (size_t i = 0; i < nCountFonts; ++i)
		{
			CString strFormat = _T("");

			strFormat.Format(_T(".font_d%u { font-family:%s; }\n"), i + 1, m_arFonts[i].Name);

			oWriter.WriteString(strFormat);
		}
		oWriter.WriteString(_T("</FontsDublicate>"));
		// -----------

		// textstyles ------
		oWriter.WriteString(_T("<TextStyles>"));
		
		for (long i = 0; i < g_ThemeTextStylesCount; ++i)
		{
			oWriter.WriteString(m_pStyles[i].ToXmlEditor(oInfo, false, i, -1));
		}
		
		oWriter.WriteString(_T("</TextStyles>"));
		// -----------------

		// background
		#ifdef PPT_DEF
		CShapeElement oElem(NSPresentationEditor::NSBaseShape::ppt, PPTShapes::sptCRect);
		#else
		#ifdef ODP_DEF
		CShapeElement oElem(NSPresentationEditor::NSBaseShape::odp, OdpShapes::sptCRect);
		oElem.m_oShape.m_dWidthLogic	= ((COdpShape*)oElem.m_oShape.m_pShape)->FManager.GetValue(_T("width"));
		oElem.m_oShape.m_dHeightLogic	= ((COdpShape*)oElem.m_oShape.m_pShape)->FManager.GetValue(_T("height"));
		#else
		CShapeElement oElem(NSPresentationEditor::NSBaseShape::pptx, OOXMLShapes::sptCRect);
		#endif
		#endif

		oElem.m_oMetric = oInfo;
		oElem.m_rcBoundsOriginal.left	= 0.0;
		oElem.m_rcBoundsOriginal.right	= (double)oInfo.m_lUnitsHor;
		oElem.m_rcBoundsOriginal.top	= 0.0;
		oElem.m_rcBoundsOriginal.bottom	= (double)oInfo.m_lUnitsVer;

		double dScaleX = (double)oInfo.m_lMillimetresHor / oInfo.m_lUnitsHor;
		double dScaleY = (double)oInfo.m_lMillimetresVer / oInfo.m_lUnitsVer;

		oElem.m_bIsBackground = true;
		oElem.m_bIsChangeable = false;

		oElem.NormalizeCoords(dScaleX, dScaleY);
		oElem.m_oShape.m_oBrush = m_oBackground;

		oWriter.WriteString(_T("<Background>"));
		oElem.SetupProperties(NULL, this, NULL);
		oWriter.WriteString(oElem.ToXmlEditor());
		oWriter.WriteString(_T("</Background>"));
		// -----------
		// elements
		oWriter.WriteString(_T("<Elements>"));

		size_t nCountElems = m_arElements.size();
		for (size_t i = 0; i < nCountElems; ++i)
		{
			m_arElements[i]->m_bIsBackground = false;
			m_arElements[i]->m_bIsChangeable = false;
			
			oWriter.WriteString(m_arElements[i]->ToXmlEditor());
		}

		oWriter.WriteString(_T("</Elements>"));
		// ----------------
		// layouts
		oWriter.WriteString(_T("<Layouts>"));

		size_t nCountLayouts = m_arLayouts.size();
		for (size_t i = 0; i < nCountLayouts; ++i)
		{
			oWriter.WriteString(m_arLayouts[i].ToXmlEditor(this, oInfo));
		}

		oWriter.WriteString(_T("</Layouts>"));

		oWriter.WriteString(_T("</Theme>"));

		return oWriter.GetData();
	}

	void NSPresentationEditor::CTheme::ReadFromXml(XmlUtils::CXmlNode& oNode)
	{
#ifdef _PRESENTATION_WRITER_
		//colors
		m_arColorScheme.clear();
		XmlUtils::CXmlNode oNodeColors;
		if (oNode.GetNode(_T("Colors"), oNodeColors))
		{
			CStylesCSS oStyles;
			oStyles.LoadStyles(oNodeColors.GetText());

			size_t nCount = oStyles.m_arStyles.size();
			LONG lColor = 0;
			for (size_t i = 0; i < nCount; i += 3)
			{
				CColor elem;
				m_arColorScheme.push_back(elem);
				oStyles.m_arStyles[i].LoadColor(m_arColorScheme[lColor]);
				++lColor;
			}
		}

		XmlUtils::CXmlNode oNodeFonts;
		if (oNode.GetNode(_T("Fonts"), oNodeFonts))
		{
			CStylesCSS oStyles;
			oStyles.LoadStyles(oNodeFonts.GetText());

			size_t nCount = oStyles.m_arStyles.size();
			for (size_t i = 0; i < nCount; ++i)
			{
				CFont elem;
				m_arFonts.push_back(elem);
				oStyles.m_arStyles[i].LoadFont(m_arFonts[i]);
			}
		}

		XmlUtils::CXmlNode oNodeTextStyles;
		if (oNode.GetNode(_T("TextStyles"), oNodeTextStyles))
		{
			XmlUtils::CXmlNodes oStyles;
			if (oNodeTextStyles.GetNodes(_T("Style"), oStyles))
			{
				int nCount = oStyles.GetCount();

				if (nCount >= g_ThemeTextStylesCount)
				{
					for (int i = 0; i < g_ThemeTextStylesCount; ++i)
					{
						XmlUtils::CXmlNode oNodeStyle;
						oStyles.GetAt(i, oNodeStyle);

						int nNumberStyle = oNodeStyle.ReadAttributeInt(_T("type"));

						CStylesCSS oCSS;
						oCSS.LoadStyles(oNodeStyle.GetText());

						if (20 != oCSS.m_arStyles.size())
							continue;

						for (int nIndexStyle = 0; nIndexStyle < 10; ++nIndexStyle)
						{
							m_pStyles[i].m_pLevels[nIndexStyle] = new CTextStyleLevel();
							
							oCSS.m_arStyles[2 * nIndexStyle].LoadStylePF(m_pStyles[i].m_pLevels[nIndexStyle]->m_oPFRun, m_oInfo);
							oCSS.m_arStyles[2 * nIndexStyle + 1].LoadStyleCF(m_pStyles[i].m_pLevels[nIndexStyle]->m_oCFRun, m_oInfo);
						}
					}
				}
			}
		}

		// background
		m_bIsBackground = false;
		XmlUtils::CXmlNode oNodeBackround;
		if (oNode.GetNode(_T("Background"), oNodeBackround))
		{
			m_bIsBackground = true;
			XmlUtils::CXmlNode oNodeMem;
			if (oNodeBackround.GetNode(_T("shape"), oNodeMem))
			{
				CShapeElement oElem;
				oElem.m_oMetric = m_oInfo;
				oElem.LoadFromXmlNode2(oNodeMem);

				m_oBackground = oElem.m_oShape.m_oBrush;
			}				
		}

		// Elements
		XmlUtils::CXmlNode oNodeElements;
		if (oNode.GetNode(_T("Elements"), oNodeElements))
		{
			XmlUtils::CXmlNodes oNodes;
			if (oNodeElements.GetNodes(_T("*"), oNodes))
			{
				int nCount = oNodes.GetCount();
				for (int i = 0; i < nCount; ++i)
				{
					XmlUtils::CXmlNode oNodeMem;
					oNodes.GetAt(i, oNodeMem);

					CShapeElement* pShapeEl = new CShapeElement();
					pShapeEl->m_oMetric = m_oInfo;
					pShapeEl->LoadFromXmlNode2(oNodeMem);

					pShapeEl->m_pTheme = this;

					m_arElements.push_back(pShapeEl);
				}
			}
		}

		XmlUtils::CXmlNode oNodeLayouts;
		if (oNode.GetNode(_T("Layouts"), oNodeLayouts))
		{
			XmlUtils::CXmlNodes oLayouts;
			if (oNodeLayouts.GetNodes(_T("Layout"), oLayouts))
			{
				int nCountLayouts = oLayouts.GetCount();
				for (int nL = 0; nL < nCountLayouts; ++nL)
				{
					XmlUtils::CXmlNode oNodeL;
					oLayouts.GetAt(nL, oNodeL);
					
					CLayout elem;
					m_arLayouts.push_back(elem);
					
					m_arLayouts.back().SetMetricInfo(m_oInfo);
					m_arLayouts.back().ReadFromXml(oNodeL);
				}
			}
		}
#endif
	}
}