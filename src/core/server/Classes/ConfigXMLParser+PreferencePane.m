#import "ConfigXMLParser.h"
#import "bridge.h"

@implementation ConfigXMLParser (PreferencePane)

- (NSMutableArray*) traverse_item:(NSXMLElement*)element stringForFilter:(NSString*)stringForFilter
{
  NSMutableArray* array = [[NSMutableArray new] autorelease];

  NSUInteger count = [element childCount];
  for (NSUInteger i = 0; i < count; ++i) {
    NSXMLElement* e = [self castToNSXMLElement:[element childAtIndex:i]];
    if (! e) continue;

    if (! [[e name] isEqualToString:@"item"]) {
      [array addObjectsFromArray:[self traverse_item:e stringForFilter:stringForFilter]];

    } else {
      NSMutableDictionary* dict = [[NSMutableDictionary new] autorelease];

      // ----------------------------------------
      NSString* title = @"";
      NSUInteger height = 0;
      for (NSXMLElement* element_name in [e elementsForName : @"name"]) {
        title = [title stringByAppendingString:[NSString stringWithFormat:@"%@\n", [element_name stringValue]]];
        ++height;
      }
      for (NSXMLElement* element_appendix in [e elementsForName : @"appendix"]) {
        title = [title stringByAppendingString:[NSString stringWithFormat:@"  %@\n", [element_appendix stringValue]]];
        ++height;
      }

      if (height == 0) {
        @throw [NSException
              exceptionWithName: @"<item> is invalid"
              reason:[NSString stringWithFormat:@"At least one <name> is necessary under <item>.\n%@", [e XMLString]]
              userInfo: nil];
      }

      title = [title stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
      [dict setObject:title forKey:@"name"];
      [dict setObject:[NSNumber numberWithUnsignedInteger:height] forKey:@"height"];

      NSString* newStringForFilter = [NSString stringWithFormat:@"%@ %@", stringForFilter, [title lowercaseString]];
      [dict setObject:newStringForFilter forKey:@"string_for_filter"];

      // ----------------------------------------
      NSArray* elements_identifier = [e elementsForName:@"identifier"];
      if ([elements_identifier count] == 1) {
        NSXMLElement* element_identifier = [elements_identifier objectAtIndex:0];
        [dict setObject:[element_identifier stringValue] forKey:@"identifier"];

        NSXMLNode* attr_default = [element_identifier attributeForName:@"default"];
        if (attr_default) {
          [dict setObject:[attr_default stringValue] forKey:@"default"];
        } else {
          [dict setObject:@"0" forKey:@"default"];
        }

        NSXMLNode* attr_step = [element_identifier attributeForName:@"step"];
        if (attr_step) {
          [dict setObject:[NSNumber numberWithInt:[[attr_step stringValue] intValue]] forKey:@"step"];
        } else {
          [dict setObject:[NSNumber numberWithInt:1] forKey:@"step"];
        }

        NSXMLNode* attr_baseunit = [element_identifier attributeForName:@"baseunit"];
        if (attr_baseunit) {
          [dict setObject:[attr_baseunit stringValue] forKey:@"baseunit"];
        }

      } else if ([elements_identifier count] >= 2) {
        @throw [NSException exceptionWithName : @"<item> is invalid" reason :[NSString stringWithFormat:@"multiple <identifier> in one <item>.\n%@", [e XMLString]] userInfo : nil];
      }

      // ----------------------------------------
      NSMutableArray* a = [[NSMutableArray new] autorelease];
      [a addObjectsFromArray:[self traverse_item:e stringForFilter:newStringForFilter]];
      if ([a count] > 0) {
        [dict setObject:a forKey:@"children"];
      }

      // ----------------------------------------
      [array addObject:dict];
    }
  }

  return array;
}

- (BOOL) reload_preferencepane {
  BOOL retval = NO;

  if (preferencepane_checkbox_) {
    [preferencepane_checkbox_ release];
  }
  preferencepane_checkbox_ = [NSMutableArray new];

  if (preferencepane_number_) {
    [preferencepane_number_ release];
  }
  preferencepane_number_ = [NSMutableArray new];

  NSArray* paths = [self get_xml_paths];
  for (NSArray* pathinfo in paths) {
    NSString* xmlpath           = [pathinfo objectAtIndex:0];
    NSNumber* xmltype           = [pathinfo objectAtIndex:1];

    @try {
      if ([xmlpath length] == 0) continue;

      NSURL* url = [NSURL fileURLWithPath:xmlpath];
      NSError* error = nil;
      NSXMLDocument* xmldocument = [[[NSXMLDocument alloc] initWithContentsOfURL:url options:0 error:&error] autorelease];
      if (! xmldocument) {
        @throw [NSException exceptionWithName :[NSString stringWithFormat:@"%@ is invalid", xmlpath] reason :[error localizedDescription] userInfo : nil];
      }

      NSMutableArray* targetarray = nil;
      if ([xmltype intValue] == CONFIGXMLPARSER_XML_TYPE_CHECKBOX) {
        targetarray = preferencepane_checkbox_;
      } else if ([xmltype intValue] == CONFIGXMLPARSER_XML_TYPE_NUMBER) {
        targetarray = preferencepane_number_;
      }
      if (targetarray) {
        [targetarray addObjectsFromArray:[self traverse_item:[xmldocument rootElement] stringForFilter:@""]];
      }

      // Set retval to YES if only one XML file is loaded successfully.
      // Unless we do it, all setting becomes disabled by one error.
      // (== If private.xml is invalid, system wide checkbox.xml is not loaded in kext.)
      retval = YES;

    } @catch (NSException* exception) {
      [self setErrorMessage:exception xmlpath:xmlpath];
    }
  }

  return retval;
}

@end
