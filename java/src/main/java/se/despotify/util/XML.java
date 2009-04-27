package se.despotify.util;

import org.w3c.dom.Document;
import org.xml.sax.InputSource;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import java.io.*;
import java.nio.charset.Charset;

public class XML {
	public static XMLElement load(Reader xml){
		/* Document and elements */
		DocumentBuilder documentBuilder = null;
		Document        document        = null;
		
		/* Create document. */
		try{
			documentBuilder = DocumentBuilderFactory.newInstance().newDocumentBuilder();
			document        = documentBuilder.parse(new InputSource(xml));
		}
		catch(Exception e){
			return null;
		}
		
		/* Return root element. */
		return new XMLElement(document.getDocumentElement());
	}
	
	public static XMLElement load(File xml) throws FileNotFoundException{
		return load(new FileReader(xml));
	}
	
	public static XMLElement load(String xml){
		return load(new StringReader(xml));
	}
	
	public static XMLElement load(byte[] xml, Charset charset){
		return load(new String(xml, charset));
	}
}
