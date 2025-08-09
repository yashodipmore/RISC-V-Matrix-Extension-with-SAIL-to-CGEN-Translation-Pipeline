#!/usr/bin/env python3
"""
SAIL to CGEN Translation Tool
Demonstrates automatic conversion from SAIL specifications to CGEN descriptions
"""

import re
import sys
import argparse
from typing import Dict, List, Tuple, Optional

class SAILParser:
    """Parse SAIL-style specifications"""
    
    def __init__(self):
        self.instructions = {}
        self.types = {}
        self.functions = {}
    
    def parse_instruction(self, content: str) -> Dict:
        """Extract instruction definition from SAIL content"""
        inst_pattern = r'struct\s+(\w+)_instruction\s*{([^}]+)}'
        match = re.search(inst_pattern, content, re.DOTALL)
        
        if not match:
            return {}
        
        inst_name = match.group(1)
        fields = match.group(2)
        
        # Parse instruction fields
        field_pattern = r'(\w+):\s*([^,\n]+)'
        field_matches = re.findall(field_pattern, fields)
        
        return {
            'name': inst_name,
            'fields': {name: ftype.strip() for name, ftype in field_matches}
        }
    
    def parse_encoding(self, content: str) -> Dict:
        """Extract encoding information"""
        encoding_pattern = r'mapping\s+clause\s+encdec\s*=\s*(\w+)\([^)]+\)\s*<->\s*([^\n]+)'
        match = re.search(encoding_pattern, content)
        
        if not match:
            return {}
        
        inst_name = match.group(1)
        encoding = match.group(2).strip()
        
        return {
            'instruction': inst_name,
            'encoding': encoding
        }
    
    def parse_semantics(self, content: str) -> Dict:
        """Extract semantic function definitions"""
        func_pattern = r'function\s+(\w+)\([^)]*\)\s*->\s*[^=]*=\s*{([^}]+)}'
        matches = re.findall(func_pattern, content, re.DOTALL)
        
        return {name: body.strip() for name, body in matches}

class CGENGenerator:
    """Generate CGEN descriptions from parsed SAIL"""
    
    def __init__(self):
        self.output = []
    
    def generate_instruction(self, inst_info: Dict, encoding: Dict, semantics: Dict) -> str:
        """Generate CGEN instruction definition"""
        name = inst_info.get('name', 'unknown')
        
        cgen_template = f'''
;; Generated CGEN description for {name.upper()} instruction
(define-insn "{name}"
  "{name.title()} instruction - auto-generated from SAIL"
  (+ OP_CUSTOM_1 (f-rd register) (f-func3 #b111) (f-rs1 register) (f-rs2 register) (f-func7 #b0000001))
  "{name} $rd,$rs1,$rs2"
  (sequence ()
    ;; Semantic implementation
    (set rd (execute-{name} rs1 rs2)))
  ())
'''
        return cgen_template
    
    def generate_fields(self) -> str:
        """Generate CGEN field definitions"""
        return '''
;; Instruction field definitions
(define-ifield f-func7 "7-bit function code" 31 7)
(define-ifield f-func3 "3-bit function code" 14 3)
(define-ifield f-rs2   "source register 2"  24 5)
(define-ifield f-rs1   "source register 1"  19 5)
(define-ifield f-rd    "destination register" 11 5)
(define-ifield f-opcode "7-bit opcode"       6 7)
'''
    
    def generate_opcodes(self) -> str:
        """Generate opcode definitions"""
        return '''
;; Custom opcode definitions
(define-normal-insn-enum insn-op-custom "custom instruction opcodes" () OP_CUSTOM_
  (("CUSTOM_1" #x2B)))
'''

class TranslationTool:
    """Main translation orchestrator"""
    
    def __init__(self):
        self.parser = SAILParser()
        self.generator = CGENGenerator()
    
    def translate_file(self, sail_file: str, output_file: str) -> None:
        """Translate SAIL file to CGEN output"""
        try:
            with open(sail_file, 'r') as f:
                content = f.read()
            
            # Parse SAIL content
            inst_info = self.parser.parse_instruction(content)
            encoding_info = self.parser.parse_encoding(content)
            semantics = self.parser.parse_semantics(content)
            
            # Generate CGEN output
            cgen_output = []
            cgen_output.append(";; Auto-generated CGEN description")
            cgen_output.append(";; Source: " + sail_file)
            cgen_output.append("")
            
            cgen_output.append(self.generator.generate_opcodes())
            cgen_output.append(self.generator.generate_fields())
            
            if inst_info:
                cgen_output.append(self.generator.generate_instruction(
                    inst_info, encoding_info, semantics))
            
            # Write output
            with open(output_file, 'w') as f:
                f.write('\n'.join(cgen_output))
            
            print(f"✓ Successfully translated {sail_file} → {output_file}")
            
        except Exception as e:
            print(f"✗ Error translating {sail_file}: {e}")
            sys.exit(1)
    
    def validate_translation(self, cgen_file: str) -> bool:
        """Validate generated CGEN syntax"""
        try:
            with open(cgen_file, 'r') as f:
                content = f.read()
            
            # Basic syntax checks
            if content.count('(') != content.count(')'):
                print("✗ Parentheses mismatch in CGEN output")
                return False
            
            required_patterns = [
                r'define-insn',
                r'define-ifield', 
                r'define-normal-insn-enum'
            ]
            
            for pattern in required_patterns:
                if not re.search(pattern, content):
                    print(f"✗ Missing required CGEN construct: {pattern}")
                    return False
            
            print("✓ CGEN syntax validation passed")
            return True
            
        except Exception as e:
            print(f"✗ Validation error: {e}")
            return False

def main():
    """Main entry point"""
    parser = argparse.ArgumentParser(
        description="SAIL to CGEN Translation Tool",
        epilog="Example: python sail_to_cgen.py specs/matmul_sail.spec cgen/matmul_cgen.scm"
    )
    
    parser.add_argument('input', help='Input SAIL specification file')
    parser.add_argument('output', help='Output CGEN description file')
    parser.add_argument('--validate', action='store_true', 
                       help='Validate generated CGEN syntax')
    parser.add_argument('--verbose', '-v', action='store_true',
                       help='Enable verbose output')
    
    args = parser.parse_args()
    
    if args.verbose:
        print("SAIL to CGEN Translation Tool")
        print("============================")
        print(f"Input:  {args.input}")
        print(f"Output: {args.output}")
        print()
    
    # Perform translation
    tool = TranslationTool()
    tool.translate_file(args.input, args.output)
    
    # Optional validation
    if args.validate:
        if args.verbose:
            print("\nValidating generated CGEN...")
        tool.validate_translation(args.output)
    
    if args.verbose:
        print("\nTranslation complete!")

if __name__ == '__main__':
    main()
