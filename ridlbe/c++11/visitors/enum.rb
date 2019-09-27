#--------------------------------------------------------------------
# @file    enum.rb
# @author  Martin Corino
#
# @brief   visitor class(es)
#
# @copyright Copyright (c) Remedy IT Expertise BV
# Chamber of commerce Rotterdam nr.276339, The Netherlands
#--------------------------------------------------------------------

module IDL
  module Cxx11

    class EnumVisitor < NodeVisitorBase

      def enumerators
        @enumerators ||= node.enumerators.collect { |en|
          (ev = visitor(EnumeratorVisitor)).visit(en)
          ev
        }
      end

      # template mapping

      map_template :enum, :enum
      map_template :typecode, :typecode
      map_template :tao_typecode, :enum_typecode

    end

    class EnumeratorVisitor < NodeVisitorBase

      def value
        node.value
      end

    end

  end
end
